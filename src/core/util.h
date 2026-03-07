#pragma once

#include <common.h>

#include "config.h"
#include "window.h"

// Template Functions for often reused Function calls searched for and created with help of ChatGPT

//Non-owning pointer guard
//Helps counter current issues with Pointers no longer existing.
template<typename T>
T& must(T* p, const char* msg) {
    assert(p && msg);
    return *p;
}

//Rect hit-test helper
inline bool point_in_rect(glm::vec2 p, glm::vec2 center, glm::vec2 size) {
    return center.x - size.x/2 <= p.x && p.x <= center.x + size.x/2
        && center.y - size.y/2 <= p.y && p.y <= center.y + size.y/2;
}

//Distance helper
inline float dist(glm::vec2 a, glm::vec2 b) {
    glm::vec2 d = a - b;
    return std::sqrt(d.x*d.x + d.y*d.y);
}
