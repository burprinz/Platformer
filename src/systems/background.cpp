//
// Created by luisb on 08.03.2026.
//

#include "background.h"

#include <iostream>

#include "fmt/compile.h"

BackgroundSystem BackgroundSystem::init(Window *window, Registry *registry, Draw *draw) noexcept {
    BackgroundSystem self;

    self.m_window = window;
    self.m_registry = registry;
    self.m_draw = draw;

    self.m_background_textures.push_back(Texture::init(assets::Texture::background_1));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_2));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_3));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_4));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_5));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_6));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_7));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_8));
    self.m_background_textures.push_back(Texture::init(assets::Texture::background_9));


    return self;
}

void BackgroundSystem::deinit() noexcept {

}

void BackgroundSystem::reset() noexcept {

}

void BackgroundSystem::step(const float delta) noexcept {
    (void) delta;

    glm::vec2 screen_size = {config::CAMERA_VIEW_WIDTH, config::CAMERA_VIEW_HEIGHT};
    glm::vec2 draw_size = screen_size * 0.5f;
    glm::vec2 camera_pos = m_registry->camera_pos;


    float speed = 0.1f;
    for (unsigned long long i = 0; i < m_background_textures.size(); i++) {
        Texture& text = m_background_textures[i];
        float parallax_factor = speed * (i+1);
        float x_position = camera_pos.x * parallax_factor;
        int multiplier = std::floor(x_position / screen_size.x);
        float origin_positions[3] = {
            (multiplier-1) * screen_size.x,
            (multiplier) * screen_size.x,
            (multiplier+1) * screen_size.x,
        };
        for (float x : origin_positions) {
            float actual_x = x + (camera_pos.x - x_position);
            m_draw->drawTexture(text, {actual_x, 0}, draw_size);
        }
    }
}
