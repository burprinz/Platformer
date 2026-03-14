#include <iostream>
#include "world.h"

#include "common.h"
#include "utils/geometry.h"

struct PlatformDef {
    glm::vec2 pos;
    glm::vec2 size;
};

WorldSystem WorldSystem::init(Window *window, Registry *registry, AudioSystem *audio_engine) noexcept {
    WorldSystem self;

    self.m_window = window;
    self.m_registry = registry;

    self.m_audio_engine = audio_engine;

    self.m_random_engine = std::default_random_engine(std::random_device()());

    self.m_reset = true;


    return self;
}

void WorldSystem::deinit() noexcept {
}

void WorldSystem::reset() noexcept {
    m_reset = false;

    /*
    entt::entity platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(0.0f, 0.0f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(2.0f, 0.1f));

    platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform, true, true, false);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(0.0f, 0.1f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(0.1f, 2.0f));

    platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(0.0f, 0.2f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(0.4f, 0.1f));

    platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(0.7f, 0.4f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(0.4f, 0.1f));

    platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform, false, false, true);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(1.4f, 0.8f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(0.4f, 0.1f));

    platform = m_registry->ecs.create();
    m_registry->ecs.emplace<Platform>(platform);
    m_registry->ecs.emplace<Position>(platform, glm::vec2(0.7f, 1.35f));
    m_registry->ecs.emplace<Dimension>(platform, glm::vec2(0.3f, 0.1f));

    entt::entity polygon = m_registry->ecs.create();
    PolygonShape poly = createPolygon({glm::vec2(0,0), glm::vec2(1,-0.5), glm::vec2(2,0.7), glm::vec2(1.5,1.3), glm::vec2(0.6,0.7)});
    m_registry->ecs.emplace<PolygonShape>(polygon, poly);
    m_registry->ecs.emplace<Position>(polygon, glm::vec2(0.0f, 0.0f));
*/
}



void WorldSystem::step(const float delta) noexcept {
    std::string title = fmt::format("Reflections - FPS: {:.2f} ({:.2f} ms)", 1 / delta, 1000 * delta);
    m_window->setTitle(title.c_str());
}
