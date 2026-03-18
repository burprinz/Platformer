#include "render.h"
#include "config.h"
#include "core/registry.h"

#include <iostream>

#include "utils/geometry.h"


constexpr int LIGHT_EFFECT_FRAMECOUNT = 32;          // number of frames in light effect texture
constexpr float LIGHT_EFFECT_ANIMATION_SPEED = 0.1f;        // time (in seconds) per frame
constexpr float SHADOW_EXTEND_DISTANCE = 100.0f;  // distance of shadow cast



RenderSystem RenderSystem::init(Window *window, Registry *registry, Draw* draw) noexcept {
    RenderSystem self;

    self.m_window = window;
    self.m_registry = registry;
	self.m_draw = draw;

	self.m_test_room_texture = Texture::init(assets::Texture::test_room);
	self.player_idle_texture = Texture::init(assets::Texture::idle);

	self.player_walking_left_texture = Texture::init(assets::Texture::walking_left);
	self.player_walking_right_texture = Texture::init(assets::Texture::walking_right);
	self.player_falling_left_texture = Texture::init(assets::Texture::falling_left);
	self.player_falling_right_texture = Texture::init(assets::Texture::falling_right);
	self.player_climbing_left_texture = Texture::init(assets::Texture::climbing_left);
	self.player_climbing_right_texture = Texture::init(assets::Texture::climbing_right);
	self.player_striking_left_texture = Texture::init(assets::Texture::striking_left);
	self.player_striking_right_texture = Texture::init(assets::Texture::striking_right);
	self.player_striking_up_texture = Texture::init(assets::Texture::striking_up);
	self.player_striking_down_texture = Texture::init(assets::Texture::striking_down);

    return self;
}

void RenderSystem::deinit() noexcept {
	m_test_room_texture.deinit();
}

void RenderSystem::reset() noexcept {

}

void RenderSystem::step(const float /*delta*/) noexcept {

	glm::vec2 image_scaling = {config::CAMERA_VIEW_WIDTH*0.5f * 2.0f, config::CAMERA_VIEW_HEIGHT*0.5f};
	for (entt::entity e : m_registry->ecs.view<RoomTexture>()) {
		RoomTexture room_text = m_registry->ecs.get<RoomTexture>(e);
		m_draw->drawTexture(room_text.texture, room_text.pos, image_scaling);
	}

	//m_draw->drawTexture(m_test_room_texture, glm::vec2(0,0), glm::vec2(config::CAMERA_VIEW_WIDTH/2.f,config::CAMERA_VIEW_HEIGHT/2.f));

	// Draw Player
	glm::vec2 player_pos = m_registry->ecs.get<Position>(m_registry->player()).pos;        // Position of the texture
	glm::vec2 player_size = m_registry->ecs.get<Dimension>(m_registry->player()).dim;
	//glm::vec2 player_vel = m_registry->ecs.get<Velocity>(m_registry->player()).vel;
	MobState player_state = m_registry->ecs.get<MobState>(m_registry->player());
	//AttackState attack_state = m_registry->ecs.get<AttackState>(m_registry->player());
	glm::vec3 color = {0, 0, 0};
	if (player_state.climbing) {
		color = {0, 0, 0};
	}
/*
	Texture current_player_texture = player_idle_texture;
	if (attack_state.state == ATTACKING) {
		switch (attack_state.attack_dir) {
			case UP:
				current_player_texture = player_striking_up_texture;
				break;
			case DOWN:
				current_player_texture = player_striking_down_texture;
				break;
			case LEFT:
				current_player_texture = player_striking_left_texture;
				break;
			case RIGHT:
				current_player_texture = player_striking_right_texture;
				break;
		}
	} else {
		if (player_state.in_air) {
			if (player_vel.x < 0) current_player_texture = player_falling_left_texture;
			else if (player_vel.x > 0) current_player_texture = player_falling_right_texture;
		} else if (player_state.climbing) {
			if (player_state.on_left_wall) current_player_texture = player_climbing_left_texture;
			else if (player_state.on_right_wall) current_player_texture = player_climbing_right_texture;
		} else {
			if (player_vel.x < 0) current_player_texture = player_walking_left_texture;
			else if (player_vel.x > 0) current_player_texture = player_walking_right_texture;
		}
	}

	// Draw Plattform
		{
			for (entt::entity platform_entity: m_registry->ecs.view<Platform>()) {
				Platform pl = m_registry->ecs.get<Platform>(platform_entity);
				glm::vec2 pos1 = m_registry->ecs.get<Position>(platform_entity).pos;
				glm::vec2 size = m_registry->ecs.get<Dimension>(platform_entity).dim;

				glm::vec2 pos2 = pos1 + glm::vec2{0, size.y};
				glm::vec2 pos3 = pos1 + glm::vec2{size.x, 0};
				glm::vec2 pos4 = pos3 + glm::vec2{0, size.y};

				//float opacity = 1.f;
				color = glm::vec3{ 1.f, 0.f, 0.f };
				if (!pl.touchable) {
					color = {1.f,1.f,0.f};
				} else if (!pl.can_climb) {
					color = {0.1176f,0.349f,0.2706f};
				}

				m_draw->drawRect(pos1, pos2, pos3, pos4, color);
			}
		}

	*/
	//m_draw->drawTexture(current_player_texture, player_pos, glm::vec2(player_size.x, player_size.y));
	m_draw->drawRect(player_pos+glm::vec2(0,player_size.y), player_pos+player_size,player_pos, player_pos+glm::vec2(player_size.x,0),color);

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

	// Draw Platform Textures
	{
		for (entt::entity platform_entity : m_registry->ecs.view<Platform>()) {
			Texture text = m_registry->ecs.get<Texture>(platform_entity);
			glm::vec2 pos = m_registry->ecs.get<Position>(platform_entity).pos;
			glm::vec2 size = m_registry->ecs.get<Dimension>(platform_entity).dim * 0.5f;
			m_draw->drawTexture(text, pos, size);
		}
	}

	// Draw Hitboxes
	{
		if (m_registry->debug) {
			for (entt::entity hitbox_entity : m_registry->ecs.view<Hitbox>()) {
				glm::vec2 pos1 = m_registry->ecs.get<Position>(hitbox_entity).pos;
				glm::vec2 size = m_registry->ecs.get<Dimension>(hitbox_entity).dim;
				glm::vec2 pos2 = pos1 + glm::vec2{0, size.y};
				glm::vec2 pos3 = pos1 + glm::vec2{size.x, 0};
				glm::vec2 pos4 = pos3 + glm::vec2{0, size.y};

				m_draw->drawRect(pos1, pos2, pos3, pos4, {1,0,0});
			}
		}
	}




	for (entt::entity polygon_entity : m_registry->ecs.view<PolygonShape>()) {
		PolygonShape poly = m_registry->ecs.get<PolygonShape>(polygon_entity);
		glm::vec2 pos = m_registry->ecs.get<Position>(polygon_entity).pos;

		glm::vec3 col = {1,0,0};
		if (rectanglePolygonCollision(Rect(player_pos, player_size), poly, pos)) col = {0,1,0};

		m_draw->fillPolygon(pos, glm::vec2{1,1}, poly, col);
	}
}