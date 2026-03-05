#include "render.h"
#include "config.h"
#include "core/registry.h"

#include <iostream>


constexpr int LIGHT_EFFECT_FRAMECOUNT = 32;          // number of frames in light effect texture
constexpr float LIGHT_EFFECT_ANIMATION_SPEED = 0.1f;        // time (in seconds) per frame
constexpr float SHADOW_EXTEND_DISTANCE = 100.0f;  // distance of shadow cast



RenderSystem RenderSystem::init(Window *window, Registry *registry, ParticleSystem* particles, Draw* draw) noexcept {
    RenderSystem self;

    self.m_window = window;
    self.m_registry = registry;
	self.m_particles = particles;
	self.m_draw = draw;

    self.m_registry->m_viewport.m_origin = glm::uvec2(0);
	self.m_registry->m_viewport.m_size = self.m_window->windowExtent();

	glm::uvec2 extent = self.m_window->windowExtent();
	self.m_intermediate_framebuffer = Framebuffer::init({ static_cast<GLsizei>(extent.x), static_cast<GLsizei>(extent.y), 1, true });



    return self;
}

void RenderSystem::deinit() noexcept {
	m_intermediate_framebuffer.deinit();
}

void RenderSystem::reset() noexcept {

}

void RenderSystem::step(const float /*delta*/) noexcept {
    Viewport viewport = m_registry->m_viewport;


	// Camera-derived projection (world scrolling)
	const auto& cam = m_registry->m_camera;
	glm::vec2 half = 0.5f * cam.viewSize;
	glm::mat4 view = m_draw->getViewMatrix();
	glm::mat4 projection = glm::ortho(
		cam.center.x - half.x, cam.center.x + half.x,
		cam.center.y - half.y, cam.center.y + half.y,
		-1.f, 1.f
	);

    glm::uvec2 extent = m_intermediate_framebuffer.extent();
    glViewport(0, 0, extent.x, extent.y);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // --- Draw player (FIXED: actually draws and closes scope)
    // render scene


	// Draw Player
	glm::vec2 player_pos = m_registry->ecs.get<Position>(m_registry->player()).pos;        // Position of the texture
	glm::vec2 player_size = m_registry->ecs.get<Dimension>(m_registry->player()).dim;
	m_draw->fillRect(player_pos+glm::vec2(0,player_size.y), player_pos+player_size,player_pos, player_pos+glm::vec2(player_size.x,0),glm::vec3(0,1,0), 1, view, projection);


    // Draw Plattform
    {
        for (entt::entity platform_entity: m_registry->ecs.view<Platform>()) {
            glm::vec2 pos1 = m_registry->ecs.get<Position>(platform_entity).pos;
            glm::vec2 size = m_registry->ecs.get<Dimension>(platform_entity).dim;

            glm::vec2 pos2 = pos1 + glm::vec2{0, size.y};
            glm::vec2 pos3 = pos1 + glm::vec2{size.x, 0};
            glm::vec2 pos4 = pos3 + glm::vec2{0, size.y};

        	float opacity = 1.f;
        	glm::vec3 color = glm::vec3{ 1.f, 0.f, 0.f };

        	m_draw->fillRect(pos1, pos2, pos3, pos4, color, opacity, view, projection);
		}
    	//m_registry->hitting.clear();
	}

    glViewport(viewport.m_origin.x, viewport.m_origin.y,
               viewport.m_size.x, viewport.m_size.y);
}



static std::pair<glm::uvec2, glm::uvec2> computeViewportConfig(const glm::uvec2 resolution) noexcept {
    float window_aspect_ratio = (float) resolution.x / resolution.y;
    if (window_aspect_ratio > config::GAME_ASPECT_RATIO) {
        glm::uvec2 viewport_size = {resolution.y * config::GAME_ASPECT_RATIO, resolution.y};
        uint32_t offset = (resolution.x - viewport_size.x) / 2;
        glm::uvec2 viewport_offset = {offset, 0};

        return {viewport_offset, viewport_size};
    }

    glm::uvec2 viewport_size = {resolution.x, resolution.x / config::GAME_ASPECT_RATIO};
    uint32_t offset = (resolution.y - viewport_size.y) / 2;
    glm::uvec2 viewport_offset = {0, offset};

    return {viewport_offset, viewport_size};
}


void RenderSystem::onResizeCallback(GLFWwindow *, int width, int height) noexcept {
    auto [origin, size] = computeViewportConfig({width, height});
	m_registry->m_viewport.m_origin = origin;
    m_registry->m_viewport.m_size = size;

    // reinitialize off-screen framebuffer
    m_intermediate_framebuffer.deinit();
    m_intermediate_framebuffer = Framebuffer::init({(GLsizei) size.x, (GLsizei) size.y, 1, true});
}
