#pragma once

#include <common.h>
#include <entt/entt.hpp>

struct Player {
    float velocity = .5f;
    float sprint_velocity = 1.f;
    float sneak_velocity = .2f;
    bool falling = true;
    float max_health = 100.f;
};

struct Position {
    glm::vec2 pos;
};

struct Velocity {
    glm::vec2 vel;
};

struct Dimension {
    glm::vec2 dim;
};

struct Radius {
    float r;
};

struct BoundingBox {
    glm::vec2 min;
    glm::vec2 max;
};

struct PolygonShape {
    std::vector<glm::vec2> vertices;
    std::vector<int> triangle_indices;   // Triangulated indices (3 ints per triangle)
    BoundingBox bounding_box;       // Axis-aligned bounding box
};

struct Scale {
    glm::vec2 scale;
};


/**
 * Objects
 */

struct Platform {

};





struct CameraState {
    glm::vec2 center{0.f, 0.f};
    glm::vec2 viewSize{1.f, 1.f};

    glm::vec2 worldMin{0.f, 0.f};
    glm::vec2 worldMax{0.f, 0.f};

    float bufferLeft   = 0.f;
    float bufferRight  = 0.f;
    float bufferTop    = 0.f;
    float bufferBottom = 0.f;
};




struct Prism {
    int reflection_color = -1;
};

struct ReflectiveSurface {
};

struct LightBeamCollider {
};

struct Arrow {
    int direction;
};

struct MirrorRotatable {
};

struct MirrorIndicator {
    float opacity;
};

// NOTE: For your own project you may want to move the registry to a different file, as it grows in size.
//	 Depending on the requirements of your projects it may also make sense to have multiple registries.
class Registry {
public:
    static Registry *init() noexcept;

    std::map<std::string, bool> m_keys;

    CameraState m_camera;

    inline entt::entity player() noexcept { return m_player; }

    inline entt::entity generate_new_player() noexcept {
        m_player = ecs.create();
        return m_player;
    }

    entt::registry ecs;

private:

    entt::entity m_player;
};
