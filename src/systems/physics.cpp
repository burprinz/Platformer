#include "physics.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>

#include "config.h"
#include "world.h"


PhysicsSystem PhysicsSystem::init(Window* window, Registry* registry, AppRegistry* app, AudioSystem* audio_engine, ShapeProcessingSystem* shape_processing) noexcept {
	PhysicsSystem self;
    self.m_window = window;
	self.m_registry = registry;
	self.m_app = app;
	self.m_audio_engine = audio_engine;
	self.m_shape_processing_system = shape_processing;
	return self;
}

void PhysicsSystem::deinit() noexcept {}

void PhysicsSystem::step(const float delta) noexcept {

	handlePlayerMovement(delta);

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
			if (m_app->debug_mode) {
				std::cout<<"POLYGON-COLLISION: "//<<e
				<<" | player_pos:"<<next_pos.x<<" "<<next_pos.y
				<<" | rect: "<<rect_position.x<<" "<<rect_position.y
				<<" , "<<rect_size.x<<" "<<rect_size.y
				<<" | dist:"<<closest_dist
				<<"| delta: "<<delta<<std::endl;
			}
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

/**
 * handlePlayer handles everything related to the player (collision, movement)
 * @param delta time passed since past frame
 * @param m_registry THE REGISTRY
 * @param m_window The Window
 */
void PhysicsSystem::handlePlayerMovement(const float delta) noexcept {
	entt::entity pl = m_registry->player();

	float ground = 0.f;

	float speed = 0.f;
	if (m_app->keys["shift"]) {
		speed = m_registry->ecs.get<Player>(pl).sprint_velocity;
	} else if (m_app->keys["strg"]) {
		speed = m_registry->ecs.get<Player>(pl).sneak_velocity;
	} else {
		speed = m_registry->ecs.get<Player>(pl).velocity;
	}

	// Update player velocity
	glm::vec2 vel = m_registry->ecs.get<Velocity>(pl).vel;
	glm::vec2 pos = m_registry->ecs.get<Position>(pl).pos;
	float radius = m_registry->ecs.get<struct Radius>(pl).r;

	// Key input (A,D)
	if (m_app->keys["d"]) {
		vel.x = speed;
	} else if (m_app->keys["a"]) {
		vel.x = -speed;
	} else {
		vel.x = 0;
	}

	// Gravity
	vel.y -= 9.81 * delta;

	if (vel.y < -3.f) vel.y = -3.f;

	bool on_ground = false;
	glm::vec2 next_pos = pos + vel * delta;

	for (entt::entity e : m_registry->ecs.view<PolygonShape>()) {
		PolygonShape* poly = &m_registry->ecs.get<PolygonShape>(e);
		glm::vec2 polygon_pos = m_registry->ecs.get<Position>(e).pos;
		glm::vec2 polygon_size = m_registry->ecs.get<Dimension>(e).dim;

		std::vector<glm::vec2> world_vertices;
		for (glm::vec2 vertex : poly->vertices) {
			world_vertices.push_back(transformVertexWorld(vertex, polygon_pos, polygon_size, 0.0f));
		}
		poly->bounding_box = m_shape_processing_system->calculateBoundingBox(world_vertices);

		glm::vec2 rect_position = poly->bounding_box.min;
		glm::vec2 rect_size = poly->bounding_box.max - poly->bounding_box.min;

		checkPlayerPolygonCollision(delta, e, next_pos, vel, on_ground, radius, rect_position, rect_size, world_vertices);

	}

	pos = next_pos;

	if (!on_ground) {
		float cur_y = pos.y - radius;
		float next_y = cur_y + vel.y * delta;
		if (next_y <= ground) {
			// Below Ground
			vel.y = 0;
			pos.y = ground+radius;
			on_ground = true;
		}
	} else if (vel.y < 0) {
		vel.y = 0;
	}

	// Key input (SPACE & W)
	if (on_ground && ((!m_app->keyboardControlMode() && m_app->jump_button > 0 && m_app->keys["space"]) || (m_app->jump_button < 2 && m_app->keys["w"]))) {
		vel.y = 1.8f;
	}

	m_registry->ecs.replace<Position>(pl, pos);
	m_registry->ecs.replace<Velocity>(pl, vel);
}

void PhysicsSystem::reset() noexcept {}
