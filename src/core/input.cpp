#include "input.h"

#include "systems/world.h"

#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include "app_registry.h"

void Input::handleKey(AppRegistry& app, int key, int action,
                      const std::function<void()>& requestReset,
                      const std::function<void()>& uiBackOrPause) noexcept
{
    switch (action) {
        case GLFW_PRESS:
            switch (key) {
                case GLFW_KEY_R:
                    if (requestReset) requestReset();
                    break;

                case GLFW_KEY_F5:
                    app.debugMode() = !app.debugMode();
                    fmt::println("Debug mode: {}", app.debugMode() ? "ON" : "OFF");
                    break;

                case GLFW_KEY_F6:
                    app.keyboardControlMode() = !app.keyboardControlMode();
                    fmt::println("Control mode: {}", app.keyboardControlMode() ? "KEYBOARD" : "MOUSE");
                    break;

                case GLFW_KEY_ESCAPE:
                        if (uiBackOrPause) uiBackOrPause();
                        break;

                case GLFW_KEY_W: app.keys.insert_or_assign("w", true); break;
                case GLFW_KEY_A: app.keys.insert_or_assign("a", true); break;
                case GLFW_KEY_D: app.keys.insert_or_assign("d", true); break;
                case GLFW_KEY_Q: app.keys.insert_or_assign("q", true); break;
                case GLFW_KEY_E: app.keys.insert_or_assign("e", true); break;

                case GLFW_KEY_SPACE:         app.keys.insert_or_assign("space", true); break;
                case GLFW_KEY_LEFT_SHIFT:    app.keys.insert_or_assign("shift", true); break;
                case GLFW_KEY_LEFT_CONTROL:  app.keys.insert_or_assign("strg", true); break;
                case GLFW_KEY_LEFT:          app.keys.insert_or_assign("left", true); break;
                case GLFW_KEY_RIGHT:         app.keys.insert_or_assign("right", true); break;
                default: break;
            }
            break;

        case GLFW_RELEASE:
            switch (key) {
                case GLFW_KEY_W: app.keys.insert_or_assign("w", false); break;
                case GLFW_KEY_A: app.keys.insert_or_assign("a", false); break;
                case GLFW_KEY_D: app.keys.insert_or_assign("d", false); break;
                case GLFW_KEY_Q: app.keys.insert_or_assign("q", false); break;
                case GLFW_KEY_E: app.keys.insert_or_assign("e", false); break;

                case GLFW_KEY_SPACE:         app.keys.insert_or_assign("space", false); break;
                case GLFW_KEY_LEFT_SHIFT:    app.keys.insert_or_assign("shift", false); break;
                case GLFW_KEY_LEFT_CONTROL:  app.keys.insert_or_assign("strg", false); break;
                case GLFW_KEY_LEFT:          app.keys.insert_or_assign("left", false); break;
                case GLFW_KEY_RIGHT:         app.keys.insert_or_assign("right", false); break;
                default: break;
            }
            break;

        default:
            break;
    }
}

void Input::handleMouse(AppRegistry& app, int button, int action) noexcept
{
    switch (action) {
    case GLFW_PRESS:
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            app.keys.insert_or_assign("mouse_left", true);
        break;

    case GLFW_RELEASE:
        if (button == GLFW_MOUSE_BUTTON_LEFT)
            app.keys.insert_or_assign("mouse_left", false);
        break;

    default:
        break;
    }
}