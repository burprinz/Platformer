#pragma once

#include <common.h>
#include <./utils/command_line_options.h>

#include <./systems/systems.h>

#include <./window.h>

#include "discord.h"
#include "draw.h"
#include "systems/background.h"
#include "systems/combat.h"
#include "utils/entity_factory.h"


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

		AudioSystem* m_audio_engine;
		CameraSystem m_camera;

		WorldSystem m_world;
		PhysicsSystem m_physics;
		CombatSystem m_combat;

		RenderSystem m_render;
		BackgroundSystem m_background;

		EntityFactory m_entity_factory;

		Draw* m_draw;
		Registry* m_registry;
		Discord m_discord;

		void reset() noexcept;

		void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) noexcept;
        void onMouseCallback(GLFWwindow* window, int button, int action, int mods) noexcept;
        void onResizeCallback(GLFWwindow* window, int width, int height) noexcept;

};
