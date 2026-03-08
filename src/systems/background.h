//
// Created by luisb on 08.03.2026.
//


#include "window.h"
#include "core/draw.h"
#include "core/registry.h"


class BackgroundSystem {

public:
    BackgroundSystem() = default;
    ~BackgroundSystem() = default;

    static BackgroundSystem init(Window* window, Registry* registry, Draw* draw) noexcept;
    void deinit() noexcept;
    void step(const float delta) noexcept;
    void reset() noexcept;



private:
    Registry* m_registry;
    Window* m_window;
    Draw* m_draw;

    std::vector<Texture> m_background_textures;

};

