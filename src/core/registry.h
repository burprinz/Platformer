#pragma once

#include <common.h>
#include <entt/entt.hpp>


constexpr int IDLE = 0;
constexpr int ATTACKING = 1;

constexpr float ATTACK_LENGTH = 0.35f;

constexpr glm::vec2 PLAYER_ATTACK_HITBOX_SIZE = {0.35, 0.15};

enum Direction {LEFT, UP, RIGHT, DOWN};

struct Viewport{
    glm::uvec2 m_origin;
    glm::uvec2 m_size;
};

struct Player {
    float velocity = .5f;
    float sprint_velocity = 1.f;
    float sneak_velocity = .2f;

    glm::vec2 lastSafePosition;
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

struct Scale {
    glm::vec2 scale;
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



/**
* Characters
*/
struct MobState {
    bool on_ground = false;
    bool on_ceiling = false;
    bool on_left_wall = false;
    bool on_right_wall = false;
    bool in_air = false;
    bool falling = false;
    bool climbing = false;
    bool can_double_jump = false;
};

/**
 * Objects
 */

struct Rect {
    glm::vec2 pos;
    glm::vec2 size;
};

struct Platform {
    bool can_climb = true;
    bool touchable = true;
};


/**
 * Combat
 */
struct AttackState {
    int state = IDLE;
    float attack_timer = 0.f;
    Rect attack_box;
    Direction attack_dir;
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

    Viewport m_viewport;
    CameraState m_camera;

    bool debug_mode = false;

    void on_key_callback(int key, int action) noexcept;
    void on_mouse_callback(int key, int action) noexcept;


    inline entt::entity player() noexcept { return m_player; }

    inline entt::entity generate_new_player() noexcept {
        m_player = ecs.create();
        return m_player;
    }

    entt::registry ecs;

    std::map<std::string, bool> keys;

    std::vector<entt::entity> hit_entities;

    glm::vec2 camera_pos = {0,0};

private:

    entt::entity m_player;
};
