#include "physics.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>

#include "config.h"
#include "world.h"


PhysicsSystem PhysicsSystem::init(Window* window, Registry* registry, AudioSystem* audio_engine, ShapeProcessingSystem* shape_processing) noexcept {
	PhysicsSystem self;
    self.m_window = window;
	self.m_registry = registry;
	self.m_audio_engine = audio_engine;
	self.m_shape_processing_system = shape_processing;
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

struct PlayerCollisionVars {
	glm::vec2 pos;
	glm::vec2 vel;
	bool on_ground;
};


bool PhysicsSystem::checkPlayerPolygonCollision(
	float delta, entt::entity e,
	glm::vec2 &next_pos, glm::vec2 &vel, bool &on_ground,
	float radius, glm::vec2 rect_position, glm::vec2 rect_size, std::vector<glm::vec2> world_vertices) noexcept {
	(void) e;
	(void) delta;
	if (m_shape_processing_system->circle_rectangle_collision(next_pos, radius, rect_position, rect_size)) {
		// next player position inside bounding box -> pruefen ob polygon-collision

		float closest_dist = config::FLOAT_MAX;
		glm::vec2 closest_point = {config::FLOAT_MAX, config::FLOAT_MAX};

		size_t n = world_vertices.size();
		for (size_t i = 0; i < n; i++) {
			glm::vec2 v1 = world_vertices[i];
			glm::vec2 v2 = world_vertices[(i+1)%n];
			glm::vec2 closest_point_on_line = m_shape_processing_system->get_closest_point_on_line(next_pos, v1, v2);
			float dist = glm::distance(next_pos, closest_point_on_line);
			if (dist < closest_dist) {
				closest_dist = dist;
				closest_point = closest_point_on_line;
			}
		}
		if (closest_dist < radius) {
			/*
			if (m_app->debug_mode) {
				std::cout<<"POLYGON-COLLISION: "//<<e
				<<" | player_pos:"<<next_pos.x<<" "<<next_pos.y
				<<" | rect: "<<rect_position.x<<" "<<rect_position.y
				<<" , "<<rect_size.x<<" "<<rect_size.y
				<<" | dist:"<<closest_dist
				<<"| delta: "<<delta<<std::endl;
			}
			*/
			// Polygon Collision
			glm::vec2 dir = glm::normalize(next_pos - closest_point);
			float depth = radius - closest_dist;
			next_pos += dir*(depth+config::EPSILON);
			float in_wall = glm::dot(vel, dir);
			if (in_wall < 0.0f) {
				// player inside polygon -> vel correction
				vel -= dir * in_wall;
			}
			if (dir.y > 0.7f) {
				// Player falling on polygon -> on ground
				on_ground = true;
				vel.y = glm::max(vel.y, 0.0f);
			}
			return true;
		}
	}
	return false;
}

glm::vec2 PhysicsSystem::calculatePlayerVelocity(float delta) noexcept {

	entt::entity pl = m_registry->player();

	float speed = 0.f;
	if (m_registry->keys["shift"]) {
		speed = m_registry->ecs.get<Player>(pl).sprint_velocity;
	} else if (m_registry->keys["strg"]) {
		speed = m_registry->ecs.get<Player>(pl).sneak_velocity;
	} else {
		speed = m_registry->ecs.get<Player>(pl).velocity;
	}

	// Update player velocity
	glm::vec2 vel = m_registry->ecs.get<Velocity>(pl).vel;

	// Key input (A,D)
	if (m_registry->keys["d"]) {
		vel.x = speed;
	} else if (m_registry->keys["a"]) {
		vel.x = -speed;
	} else {
		vel.x = 0;
	}

	MobState player_state = m_registry->ecs.get<MobState>(m_registry->player());
	// Key input (SPACE & W)
	if (player_state.on_ground && (m_registry->keys["w"] || m_registry->keys["space"])) {
		vel.y = 1.8f;
	} else {
		// Gravity
		vel.y -= 9.81 * delta;
		if (vel.y < -3.f) vel.y = -3.f;
	}

	return vel;
}

void PhysicsSystem::handleCollision(float delta, entt::entity entity_id, glm::vec2 vel) noexcept {

	glm::vec2 pos = m_registry->ecs.get<Position>(entity_id).pos;
	glm::vec2 size = m_registry->ecs.get<Dimension>(entity_id).dim;

	bool on_ground = false;
	bool on_ceiling = false;
	bool on_left = false;
	bool on_right = false;

	(void) (on_ground && on_left && on_right && on_ceiling);

	glm::vec2 next_pos = pos + vel*delta;

	for (entt::entity platform_id : m_registry->ecs.view<Platform>()) {
		glm::vec2 platform_pos = m_registry->ecs.get<Position>(platform_id).pos;
		glm::vec2 platform_size = m_registry->ecs.get<Dimension>(platform_id).dim;

		if (next_pos.x <= platform_pos.x + platform_size.x &&
			next_pos.x + size.x >= platform_pos.x &&
			next_pos.y <= platform_pos.y + platform_size.y &&
			next_pos.y + size.y >= platform_pos.y) {

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
			} else if (max_dist == right_dist && vel.x < 0) {
				next_pos.x = platform_pos.x + platform_size.x;
				vel.x = 0;
				on_left = true;
			}
		}
	}

	if (next_pos.y < 0) {
		next_pos.y = 0;
		vel.y = 0;
		on_ground = true;
	}

	MobState state;
	state.on_ground = on_ground;
	state.on_ceiling = on_ceiling;
	state.on_left_wall = on_left;
	state.on_right_wall = on_right;
	state.in_air = !(on_ground || on_ceiling || on_left || on_right);

	m_registry->ecs.replace<Position>(entity_id, next_pos);
	m_registry->ecs.replace<Velocity>(entity_id, vel);
	m_registry->ecs.replace<MobState>(entity_id, state);
}


void PhysicsSystem::reset() noexcept {}
