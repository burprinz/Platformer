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

    return self;
}

void RenderSystem::deinit() noexcept {
}

void RenderSystem::reset() noexcept {

}

void RenderSystem::step(const float /*delta*/) noexcept {

	// Draw Player
	glm::vec2 player_pos = m_registry->ecs.get<Position>(m_registry->player()).pos;        // Position of the texture
	glm::vec2 player_size = m_registry->ecs.get<Dimension>(m_registry->player()).dim;
	MobState player_state = m_registry->ecs.get<MobState>(m_registry->player());
	glm::vec3 color = {0, 1, 0};
	if (player_state.climbing) {
		color = {0, 0, 1};
	}

	m_draw->drawRect(player_pos+glm::vec2(0,player_size.y), player_pos+player_size,player_pos, player_pos+glm::vec2(player_size.x,0),color);

    // Draw Plattform
    {
        for (entt::entity platform_entity: m_registry->ecs.view<Platform>()) {
            glm::vec2 pos1 = m_registry->ecs.get<Position>(platform_entity).pos;
            glm::vec2 size = m_registry->ecs.get<Dimension>(platform_entity).dim;

            glm::vec2 pos2 = pos1 + glm::vec2{0, size.y};
            glm::vec2 pos3 = pos1 + glm::vec2{size.x, 0};
            glm::vec2 pos4 = pos3 + glm::vec2{0, size.y};

        	float opacity = 1.f;
        	color = glm::vec3{ 1.f, 0.f, 0.f };

        	m_draw->fillRect(pos1, pos2, pos3, pos4, color, opacity);
		}
	}

	// Draw Attack Hitboxes
	{
		for (entt::entity attack_state_entity : m_registry->ecs.view<AttackState>()) {
			AttackState state = m_registry->ecs.get<AttackState>(attack_state_entity);
			//std::cout<<state.state<<std::endl;
			if (state.state == ATTACKING) {
				glm::vec2 entity_pos = m_registry->ecs.get<Position>(attack_state_entity).pos;
				Rect bbox = state.attack_box;
				glm::vec2 pos1 = entity_pos + bbox.pos;
				glm::vec2 pos2 = pos1 + glm::vec2{0, bbox.size.y};
				glm::vec2 pos3 = pos1 + glm::vec2{bbox.size.x, 0};
				glm::vec2 pos4 = pos3 + glm::vec2{0, bbox.size.y};
				color = glm::vec3{ 1.f, 0.f, 0.f };
				m_draw->drawRect(pos1, pos2, pos3, pos4, color);
			}
		}
	}
}
