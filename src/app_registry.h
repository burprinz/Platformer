#pragma once

#include "common.h"

struct Viewport{
    glm::uvec2 m_origin;
    glm::uvec2 m_size;
};

class AppRegistry {
public:
    static AppRegistry* init() noexcept;
    Viewport viewport;

    // Keys
    std::map<std::string, bool> keys;

    // Flags
    bool debug_mode = false;
    bool tutorial_running = false;
    int jump_button;
    bool keyboard_control_mode = false;

    //
    inline bool& debugMode() noexcept { return debug_mode; }
    inline void setTutorialRunning(bool v) noexcept { tutorial_running = v; }
    inline bool isTutorialRunning() const noexcept { return tutorial_running; }
    inline bool& keyboardControlMode() noexcept { return keyboard_control_mode; }

    void clearInput() noexcept; // optional: set all keys to false
};
