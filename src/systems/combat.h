#pragma once

#include <core/registry.h>
#include <systems/audio.h>

#include "window.h"

class CombatSystem {

public:
    static CombatSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;


    void handlePlayerAttacks(const float delta);

private:
    Window* m_window;
    Registry* m_registry;
};

