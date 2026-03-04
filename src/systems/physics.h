#pragma once

#include <registry.h>
#include <systems/ShapeProcessing.h>
#include <systems/audio.h>

#include "app_registry.h"
#include "window.h"

struct LightBeamCollision
{
	glm::vec2 collision_point;
	glm::vec2 collided_line_direction;
	bool player_inside_polygon = false;
};

class PhysicsSystem {
	public:
		static PhysicsSystem init(Window* window, Registry* registry, AppRegistry* app, AudioSystem* audio_engine, ShapeProcessingSystem* shape_processing) noexcept;
		void deinit() noexcept;

		void step(const float delta) noexcept;
		void reset() noexcept;

		void handlePlayerMovement(float delta) noexcept;

		bool checkPlayerPolygonCollision(float delta, entt::entity e,glm::vec2 &next_pos, glm::vec2 &vel, bool &on_ground,
				float radius, glm::vec2 rect_position, glm::vec2 rect_size, std::vector<glm::vec2> world_vertices) noexcept;

		inline glm::vec2 toLocalPoint(const glm::vec2& world, const glm::vec2& position, const glm::vec2& scale) {
			return (world - position) / scale;
		}

		inline glm::vec2 toLocalDirection(const glm::vec2& direction, const glm::vec2& scale) {
			return direction / scale;
		}


	private:
        Window* m_window;
		Registry* m_registry;
		AppRegistry* m_app;
		AudioSystem* m_audio_engine;
		ShapeProcessingSystem* m_shape_processing_system;
};
