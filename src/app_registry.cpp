#include "app_registry.h"
#include <glm/vec2.hpp>

AppRegistry* AppRegistry::init() noexcept {
    auto* self = new AppRegistry();

    // preserve your existing key set
    self->keys.insert({"w", false});
    self->keys.insert({"a", false});
    self->keys.insert({"d", false});
    self->keys.insert({"q", false});
    self->keys.insert({"e", false});
    self->keys.insert({"space", false});
    self->keys.insert({"shift", false});
    self->keys.insert({"strg", false});
    self->keys.insert({"mouse_left", false});

    self->viewport.m_origin = glm::uvec2(0);
    self->viewport.m_size   = glm::uvec2(0);

    self->debug_mode = false;
    self->jump_button = 1;
    self->tutorial_running = false;

    return self;
}
