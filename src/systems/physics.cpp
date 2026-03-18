#include "physics.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>

#include "core/util.h"
#include "config.h"
#include "world.h"
#include "utils/geometry.h"


PhysicsSystem PhysicsSystem::init(Window* window, Registry* registry, AudioSystem* audio_engine) noexcept {
	PhysicsSystem self;
    self.m_window = window;
	self.m_registry = registry;
	self.m_audio_engine = audio_engine;
	return self;
}

void PhysicsSystem::deinit() noexcept {}

void PhysicsSystem::step(const float delta) noexcept {


	glm::vec2 pl_vel = calculatePlayerVelocity(delta);
	handleCollision(delta, m_registry->player(), pl_vel);

	// Update position

	for (entt::entity e : m_registry->ecs.view<Velocity>()) {
		glm::vec2 pos = m_registry->ecs.get<Position>(e).pos;
		glm::vec2 vel = m_registry->ecs.get<Velocity>(e).vel;
		pos = pos + vel*delta;
		m_registry->ecs.replace<Position>(e, pos);
	}

}



glm::vec2 transformVertexWorld(
	const glm::vec2& local,
	const glm::vec2& position,
	const glm::vec2& scale,
	float angle)
{
	glm::vec2 p = local * scale;

	p += position;

	glm::vec2 center = position + scale * 0.5f;

	glm::vec2 d = p - center;

	float c = cos(angle);
	float s = sin(angle);

	glm::vec2 r{
		d.x * c - d.y * s,
		d.x * s + d.y * c
	};

	return center + r;
}


glm::vec2 PhysicsSystem::calculatePlayerVelocity(float delta) noexcept {

	entt::entity pl = m_registry->player();
	Player player = m_registry->ecs.get<Player>(pl);

	float speed = player.velocity;

	// Update player velocity
	glm::vec2 vel = m_registry->ecs.get<Velocity>(pl).vel;
	glm::vec2 pos =  m_registry->ecs.get<Position>(pl).pos;

	// Key input (A,D)
	if (m_registry->keys["d"]) {
		vel.x += delta*player.acceleration;
		if (vel.x > speed) vel.x = speed;
	} else if (m_registry->keys["a"]) {
		vel.x -= delta*player.acceleration;
		if (vel.x < -speed) vel.x = -speed;
	} else {
		vel.x = 0;
	}

	MobState player_state = m_registry->ecs.get<MobState>(m_registry->player());
	AttackState attack_state = m_registry->ecs.get<AttackState>(m_registry->player());

	//std::cout<<player_state.on_ground<<" "<<player_state.on_ceiling<<" "<<player_state.on_left_wall<<" "<<player_state.on_right_wall<<" "<<player_state.falling<<" "<<player_state.climbing<<" "<<player_state.in_air<<std::endl;


	// Player is on "spike"-platform
	for (entt::entity e : m_registry->hit_entities) {
		if (!m_registry->ecs.get<Hitbox>(e).touchable) {
			vel = {0, 0};
			m_registry->ecs.replace<Position>(pl, player.lastSafePosition);
			m_registry->hit_entities.clear();
			return vel;
		}
	}
	m_registry->hit_entities.clear();


	// Update last safe position if current pos is safe
	if (player_state.on_ground) {
		player.lastSafePosition = pos;
	}

	// Ceck if player can pogo
	if (attack_state.state == ATTACKING && player_state.in_air && attack_state.attack_dir == DOWN) {
		Rect attack_hitbox = attack_state.attack_box;
		attack_hitbox.pos += pos;
		bool pogod = false;
		for (entt::entity hitbox_id : m_registry->ecs.view<Platform>()) {
			Hitbox hb = m_registry->ecs.get<Hitbox>(hitbox_id);
			Rect hitbox = Rect(m_registry->ecs.get<Position>(hitbox_id).pos, m_registry->ecs.get<Dimension>(hitbox_id).dim);
			if (hb.can_pogo && isFullyAboveRect(pos, hitbox) && rectCollision(attack_hitbox, hitbox)) {
				player_state.can_double_jump = true;
				vel.y = JUMP_HEIGHT;
				pogod = true;
			}
		}
		if (!pogod) {
			for (entt::entity polygon : m_registry->ecs.view<PolygonShape>()) {
				PolygonShape shape = m_registry->ecs.get<PolygonShape>(polygon);
				glm::vec2 position = m_registry->ecs.get<Position>(polygon).pos;
				if (rectanglePolygonCollision(attack_hitbox, shape, position)) {
					player_state.can_double_jump = true;
					vel.y = JUMP_HEIGHT;
				}
			}
		}
	}

	if ((player_state.on_ground // On ground
		|| player_state.climbing // On wall
		|| player_state.can_double_jump // In air with double jump
		)
		&& m_registry->keys["space"]) {

		vel.y = JUMP_HEIGHT;
		if (player_state.climbing) {
			if (player_state.on_left_wall) {
				vel.x = speed;
			} else if (player_state.on_right_wall) {
				vel.x = -speed;
			}
		} else if (player_state.in_air) {
			player_state.can_double_jump = false;
		}
		m_registry->keys["space"] = false;
	} else {
		// Gravity
		if (player_state.climbing) {
			vel.y = 0;
		} else {
			vel.y -= GRAVITY * delta;
			if (vel.y < -13.f) vel.y = -13.f;
		}
	}

	m_registry->ecs.replace<MobState>(m_registry->player(), player_state);
	m_registry->ecs.replace<Player>(m_registry->player(), player);
	m_registry->ecs.replace<AttackState>(m_registry->player(), attack_state);
	return vel;
}

void PhysicsSystem::handleCollision(float delta, entt::entity entity_id, glm::vec2 vel) noexcept {

	glm::vec2 pos = m_registry->ecs.get<Position>(entity_id).pos;
	glm::vec2 size = m_registry->ecs.get<Dimension>(entity_id).dim;
	MobState state = m_registry->ecs.get<MobState>(entity_id);


	bool on_ground = false;
	bool on_ceiling = false;
	bool on_left = false;
	bool on_right = false;

	glm::vec2 next_pos = pos + vel*delta;

	entt::entity side_touch_entity = entt::null;

	for (entt::entity platform_id : m_registry->ecs.view<Hitbox>()) {
		glm::vec2 platform_pos = m_registry->ecs.get<Position>(platform_id).pos;
		glm::vec2 platform_size = m_registry->ecs.get<Dimension>(platform_id).dim;

		if (next_pos.x <= platform_pos.x + platform_size.x &&
			next_pos.x + size.x >= platform_pos.x &&
			next_pos.y <= platform_pos.y + platform_size.y &&
			next_pos.y + size.y >= platform_pos.y) {
			//m_registry->hit_entities.push_back(platform_id);

			float top_dist = pos.y - (platform_pos.y + platform_size.y);
			float bot_dist = platform_pos.y - (pos.y + size.y);
			float left_dist = platform_pos.x - (pos.x + size.x);
			float right_dist = pos.x - (platform_pos.x + platform_size.x);
			float max_dist = top_dist;
			if (bot_dist > max_dist) max_dist = bot_dist;
			if (left_dist > max_dist) max_dist = left_dist;
			if (right_dist > max_dist) max_dist = right_dist;

			if (max_dist == top_dist && vel.y < 0) {
				next_pos.y = platform_pos.y + platform_size.y;
				vel.y = 0;
				on_ground = true;
			} else if (max_dist == bot_dist && vel.y > 0) {
				next_pos.y = platform_pos.y - size.y;
				vel.y = 0;
				on_ceiling = true;
			}

			if (max_dist == left_dist && vel.x > 0) {
				next_pos.x = platform_pos.x - size.x;
				vel.x = 0;
				on_right = true;
				side_touch_entity = platform_id;
			} else if (max_dist == right_dist && vel.x < 0) {
				next_pos.x = platform_pos.x + platform_size.x;
				vel.x = 0;
				on_left = true;
				side_touch_entity = platform_id;
			}
		}
	}

	if (next_pos.y < 0) {
		next_pos.y = 0;
		vel.y = 0;
		on_ground = true;
	}

	state.on_ground = on_ground;
	state.on_ceiling = on_ceiling;
	state.on_left_wall = on_left;
	state.on_right_wall = on_right;
	state.in_air = !(on_ground || on_ceiling || on_left || on_right);
	state.falling = !state.on_ground && vel.y <= 0;
	state.climbing = state.falling && (on_left || on_right);
	if (state.on_ground || state.climbing) state.can_double_jump = true;

	if (side_touch_entity != entt::null) {
		Hitbox hb = m_registry->ecs.get<Hitbox>(side_touch_entity);
		state.climbing = state.climbing && hb.can_climb;
	}

	m_registry->ecs.replace<Position>(entity_id, next_pos);
	m_registry->ecs.replace<Velocity>(entity_id, vel);
	m_registry->ecs.replace<MobState>(entity_id, state);
}


void PhysicsSystem::reset() noexcept {}
