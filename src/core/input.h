#pragma once
#include <functional>

#include "app_registry.h"

struct GLFWwindow;


class Input {
public:
    static void handleKey(AppRegistry& app, int key, int action,
                             const std::function<void()>& requestReset,
                            const std::function<void()>& uiBackOrPause) noexcept;

    static void handleMouse(AppRegistry& app, int button, int action) noexcept;
};
