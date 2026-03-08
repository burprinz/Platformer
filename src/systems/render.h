#pragma once

#include <core/registry.h>
#include <window.h>



#include "particle.h"
#include "core/draw.h"


class RenderSystem {
	public:
		RenderSystem() = default;
		~RenderSystem() = default;

		static RenderSystem init(Window* window, Registry* registry, ParticleSystem* particles, Draw* m_draw) noexcept;
		void deinit() noexcept;
		void step(const float delta) noexcept;
		void reset() noexcept;



	private:
		Registry* m_registry;
		Window* m_window;
		ParticleSystem* m_particles;
		Draw* m_draw;


		Texture player_idle_texture;

		Texture player_walking_left_texture;
		Texture player_walking_right_texture;
		Texture player_falling_left_texture;
		Texture player_falling_right_texture;
		Texture player_climbing_left_texture;
		Texture player_climbing_right_texture;
		Texture player_striking_left_texture;
		Texture player_striking_right_texture;
		Texture player_striking_up_texture;
		Texture player_striking_down_texture;

		Texture m_test_room_texture;
};
