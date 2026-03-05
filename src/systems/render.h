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
		void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;


	private:
		Registry* m_registry;
		Window* m_window;
		ParticleSystem* m_particles;
		Draw* m_draw;

		Framebuffer m_intermediate_framebuffer;
};
