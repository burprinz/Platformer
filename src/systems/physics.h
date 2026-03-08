#pragma once

#include <core/registry.h>
#include <systems/audio.h>

#include "window.h"

constexpr float JUMP_HEIGHT = 1.2f;
constexpr float GRAVITY = 3.5f;

class PhysicsSystem {
	public:
		static PhysicsSystem init(Window* window, Registry* registry, AudioSystem* audio_engine) noexcept;
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

		glm::vec2 calculatePlayerVelocity(float delta) noexcept;
		void handleCollision(float delta, entt::entity entity_id, glm::vec2 vel) noexcept;

	private:
        Window* m_window;
		Registry* m_registry;
		AudioSystem* m_audio_engine;
};
