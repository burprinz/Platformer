//
// Created by luisb on 07.01.2026.
//

#include "particle.h"

#include <iostream>


ParticleSystem *ParticleSystem::init(Window *window, Registry *registry) noexcept {
    ParticleSystem *self = new ParticleSystem();

    self->m_window = window;
    self->m_registry = registry;

    return self;
}

void ParticleSystem::deinit() noexcept {

}

void ParticleSystem::reset() noexcept {

}


void ParticleSystem::step(const float delta) noexcept {
    for (size_t c = 0; c < m_player_dust.size(); c++) {
        DustParticle* current_particle = &m_player_dust[c];

        if (!current_particle->active) {
            // Partikel nicht vorhanden
            generateNewDustParticle(current_particle, m_registry->ecs.get<Position>(m_registry->player()).pos);
        } else {
            // Partikel vorhanden
            current_particle->position += current_particle->velocity * delta;

            if (!current_particle->fully_appeared) {
                // Opacity steigt
                current_particle->opacity += current_particle->appear_speed * delta;
                if (current_particle->opacity > current_particle->max_opacity) {
                    current_particle->opacity = current_particle->max_opacity;
                    current_particle->fully_appeared = true;
                }
            } else {
                // Opacity sinkt
                current_particle->opacity -= current_particle->vanish_speed * delta;
                if (current_particle->opacity <= 0.0f) {
                    current_particle->opacity = 0.0f;
                    current_particle->active = false;
                }
            }

        }
    }

}


void ParticleSystem::generateNewDustParticle(DustParticle* particle, glm::vec2 center) noexcept {
    float max_vel = 0.1f;
    float max_appear_val = 1.f;
    float angle = random(2*glm::pi<float>());

    float radius = m_registry->ecs.get<Radius>(m_registry->player()).r;

    float x_offset = cos(angle)*radius;
    float y_offset = sin(angle)*radius;

    float vel = random(max_vel);
    float x_vel = vel * cosf(angle);
    float y_vel = vel * sinf(angle);

    float appear_speed = random(max_appear_val);

    particle->opacity = 0.0f;
    particle->fully_appeared = false;
    particle->position = {center.x + x_offset, center.y + y_offset};
    particle->velocity = {x_vel, y_vel};
    particle->appear_speed = appear_speed;
    particle->vanish_speed = appear_speed;
    particle->active = true;
}
