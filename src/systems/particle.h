#pragma once
//
// Created by luisb on 07.01.2026.
//

#include "registry.h"
#include "window.h"

constexpr size_t MAX_PLAYER_DUST = 100;

constexpr float MIN_PARTICLE_SIZE = 0.0015f;

struct DustParticle {
    bool active = false;
    glm::vec2 position = {0.1f, 0.1f};
    glm::vec2 velocity = {0.0f, 0.0f};
    float radius = 0.005f;
    float max_opacity = 1.0f;
    float appear_speed = 0.5f;
    float vanish_speed = 0.5f;
    float opacity = 0.0f;
    bool fully_appeared = false;
};

class ParticleSystem {
public:
    ParticleSystem() = default;

    ~ParticleSystem() = default;

    static ParticleSystem *init(Window *window, Registry *registry) noexcept;
    void deinit() noexcept;
    void step(const float delta) noexcept;
    void reset() noexcept;

    void generateNewDustParticle(DustParticle* particle, glm::vec2 center) noexcept;


    /**
     * Returns a random number between 0 and range.
     * @param range
     * @return the random number
     */
    inline float random(float range) noexcept {return (float) rand() / RAND_MAX * range;};

    inline std::array<DustParticle, MAX_PLAYER_DUST> getPlayerDustParticles() {return m_player_dust;}

private:
    Registry *m_registry;
    Window *m_window;

    std::array<DustParticle, MAX_PLAYER_DUST> m_player_dust;

};
