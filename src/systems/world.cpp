#include <iostream>
#include "world.h"

#include "common.h"

struct PlatformDef {
    glm::vec2 pos;
    glm::vec2 size;
};

WorldSystem WorldSystem::init(Window *window, Registry *registry,AppRegistry *app_registry, AudioSystem *audio_engine) noexcept {
    WorldSystem self;

    self.m_window = window;
    self.m_registry = registry;
    self.m_app = app_registry;

    self.m_audio_engine = audio_engine;

    self.m_random_engine = std::default_random_engine(std::random_device()());


    self.m_reset = true;

    return self;
}

void WorldSystem::deinit() noexcept {
}

void WorldSystem::reset() noexcept {
    m_reset = false;


}



void WorldSystem::step(const float delta) noexcept {
    std::string title = fmt::format("Reflections - FPS: {:.2f} ({:.2f} ms)", 1 / delta, 1000 * delta);
    m_window->setTitle(title.c_str());
}
