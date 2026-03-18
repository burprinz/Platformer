//
// Created by luisb on 14.03.2026.
//

#include "window.h"
#include "core/registry.h"

class AnimationSystem {
public:
    static AnimationSystem init(Window* window, Registry* registry) noexcept;
    void deinit() noexcept;

    void step(const float delta) noexcept;
    void reset() noexcept;

private:
    Window* m_window;
    Registry* m_registry;


};
