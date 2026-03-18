#include "application.h"

#include <iostream>

#include "../config.h"

static void glfwErrorCallback(int error, const char* description) {
	fmt::println(stderr, "[GLFW Error {}]: {}", error, description);
}

#if !defined(__APPLE__)
static void glDebugCallback(GLenum source,
		GLenum type, GLuint id, GLenum severity,
		GLsizei , const GLchar* message, const void*) {
	(void)source;
	(void)type;
	(void)id;
	(void)severity;
	if (type == GL_DEBUG_TYPE_OTHER) return;
	fmt::println(stderr, "[GL DEBUG MESSAGE]: {}", message);
}
#endif

std::optional<Application> Application::init(const CommandLineOptions& options) noexcept {
	if (options.help()) return std::nullopt;

	Application self;
	fmt::println("\"{}\" version {}.{}", PROJECT_NAME, VERSION_MAJOR, VERSION_MINOR);

	if (options.x11()) glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
	
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit()) {
		fmt::println(stderr, "Failed to initialize GLFW");
		return std::nullopt;
	}

	std::optional<Window*> win = Window::init(1280, 720, PROJECT_NAME);
	if (!win) {
		glfwTerminate();
		return std::nullopt;
	}
	self.m_window = std::move(*win);

	if (gl3wInit()) {
		fmt::println(stderr, "Failed to initialize OpenGL context");
		self.m_window->deinit();
		glfwTerminate();
		return std::nullopt;
	}
	fmt::println("Loaded OpenGL {} & GLSL {}", (char*)glGetString(GL_VERSION), (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

#if !defined(__APPLE__)
	glDebugMessageCallback(glDebugCallback, nullptr);
#endif

	self.m_audio_engine = new AudioSystem;
	*self.m_audio_engine = AudioSystem::init();

	self.m_discord = Discord::init();
	self.m_discord.update();
	self.m_registry = Registry::init();
	self.m_draw = Draw::init(self.m_window, self.m_registry);

	self.m_entity_factory = EntityFactory::init(self.m_window, self.m_registry);

	self.m_world = WorldSystem::init(self.m_window, self.m_registry, self.m_audio_engine);
	self.m_combat = CombatSystem::init(self.m_window, self.m_registry);
	self.m_physics = PhysicsSystem::init(self.m_window, self.m_registry, self.m_audio_engine);
	self.m_render = RenderSystem::init(self.m_window, self.m_registry, self.m_draw);
	self.m_background = BackgroundSystem::init(self.m_window, self.m_registry, self.m_draw);
	self.m_animation = AnimationSystem::init(self.m_window, self.m_registry);
	self.m_camera = CameraSystem::init(self.m_registry);

	return self;
}

void Application::deinit() noexcept {
	m_world.deinit();
	m_physics.deinit();
	m_render.deinit();
	m_audio_engine->deinit();
	m_animation.deinit();

	m_draw->deinit();

	delete m_registry;
	m_window->deinit();
	delete m_window;
	glfwTerminate();
}

void Application::run() noexcept {

	ma_sound* music = m_audio_engine->backgroundMusic();
	ma_sound_set_looping(music, MA_TRUE);
	ma_sound_start(music);

	m_window->setResizeCallback([&](GLFWwindow* window, int width, int height) -> void {
			onResizeCallback(window, width, height);
			});
	m_window->setKeyCallback([&](GLFWwindow* window, int key, int scancode, int action, int mods) -> void {
			onKeyCallback(window, key, scancode, action, mods);
			});
    m_window->setMouseCallback([&](GLFWwindow* window, int key, int action, int mods) -> void {
        onMouseCallback(window, key, action, mods);
    });
	
	float delta_time = 0;
	float last_time = static_cast<float>(glfwGetTime());
	float physics_accumulator = 0.f;
	//bool in_game = false;

	glClearColor(0, 0, 0, 1);
	while (!m_window->shouldClose()) {
		glfwPollEvents();

		if (m_world.shouldReset()) reset();

		float time = static_cast<float>(glfwGetTime());
		delta_time = time - last_time;
		last_time = time;

		m_world.step(delta_time);

		m_combat.step(delta_time);

		physics_accumulator += delta_time;
		while (physics_accumulator >= config::PHYSICS_TIME_STEP) {
			m_physics.step(config::PHYSICS_TIME_STEP);
			physics_accumulator -= config::PHYSICS_TIME_STEP;
		}

		m_camera.step(delta_time);

		m_animation.step(delta_time);

		m_draw->start();
		m_background.step(delta_time);
		m_render.step(delta_time);
		m_draw->finish();

		m_window->swapBuffers();
	}
}

void Application::reset() noexcept {
	// reset all game state
	m_registry->ecs.clear();

	// reset bools
	m_registry->debug_mode = false;

	// initialize the player
	entt::entity player_id = m_registry->generate_new_player();
	glm::vec2 player_start_pos = {0.5f, 1.0f};
	m_registry->ecs.emplace<Player>(player_id);
	m_registry->ecs.emplace<Position>(player_id, player_start_pos);
	m_registry->ecs.emplace<Velocity>(player_id, glm::vec2(0.f, 0.f));
	m_registry->ecs.emplace<Scale>(player_id, glm::vec2(-0.1f, 0.1f));
	m_registry->ecs.emplace<Radius>(player_id, 0.1f);
	m_registry->ecs.emplace<Dimension>(player_id, glm::vec2(0.45, 0.765));
	m_registry->ecs.emplace<MobState>(player_id);
	m_registry->ecs.emplace<AttackState>(player_id);

	//m_entity_factory.createFromFile("test_room.json");

	m_entity_factory.createFromFile("/assets/roomSchemes/test_room.json");

	// reset systems
	m_world.reset();
	m_physics.reset();
	m_render.reset();
	m_camera.reset();
}

void Application::onKeyCallback(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) noexcept {
	// preserve “world-first” behavior: Input state is updated before UI/tut see it
	m_registry->on_key_callback(key, action);
}

void Application::onMouseCallback(GLFWwindow* /*window*/, int button, int action, int /*mods*/) noexcept {
	// Preserve old behavior: mouse_left is only updated while IN_GAME
	m_registry->on_mouse_callback(button, action);
}

void Application::onResizeCallback(GLFWwindow* window, int width, int height) noexcept {
	m_draw->onResizeCallback(window, width, height);
}