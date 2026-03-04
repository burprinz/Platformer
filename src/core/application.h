#pragma once

#include <common.h>
#include <./utils/command_line_options.h>

#include <./systems/systems.h>

#include <./window.h>

#include "app_registry.h"
#include "../systems/particle.h"


class Application {
	public:
		// NOTE: You may want to use the constructor and destructor for initialization
		//       and deinitialization of objects. For the template we opted to use explicit
		//       initialization and deinitialization to avoid hidden control flow.
		static std::optional<Application> init(const CommandLineOptions& options) noexcept;
		void deinit() noexcept;
		void run() noexcept;

	private:
		Window* m_window;

		AppRegistry* m_app;

		AudioSystem* m_audio_engine;
		CameraSystem m_camera;
		ParticleSystem* m_particles;

		WorldSystem m_world;
		PhysicsSystem m_physics;
		RenderSystem m_render;
		ShapeProcessingSystem* m_shape_processing_system;

		Registry* m_registry;

		void reset() noexcept;

		void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
        void onMouseCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
        void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;

};
