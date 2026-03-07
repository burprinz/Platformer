//
// Created by luisb on 07.03.2026.
//

#include "combat.h"

#include <iostream>

CombatSystem CombatSystem::init(Window* window, Registry* registry) noexcept {
    CombatSystem self;
    self.m_window = window;
    self.m_registry = registry;
    return self;
}

void CombatSystem::deinit() noexcept {}

void CombatSystem::reset() noexcept {}

void CombatSystem::step(const float delta) noexcept {
    handlePlayerAttacks(delta);
}

void CombatSystem::handlePlayerAttacks(const float delta) {
    entt::entity player_id = m_registry->player();
    MobState mob_state = m_registry->ecs.get<MobState>(player_id);
    AttackState attack_state = m_registry->ecs.get<AttackState>(player_id);

    if (attack_state.state == ATTACKING) {
        attack_state.attack_timer += delta;
        if (attack_state.attack_timer > ATTACK_LENGTH) {
            attack_state.state = IDLE;
            attack_state.attack_timer = 0;
        }
    } else if (attack_state.state == IDLE) {

        if (m_registry->keys["mouse_left"]) {
            glm::vec2 player_pos = m_registry->ecs.get<Position>(player_id).pos;
            glm::vec2 player_size = m_registry->ecs.get<Dimension>(player_id).dim;
            glm::vec2 player_midpoint = player_pos + player_size * 0.5f;

            float mouse_angle = m_window->calcRelativeMouseAngleInDeg(player_midpoint);
            glm::vec2 hit_pos;
            Direction hit_dir;
            if (mouse_angle >= 45 && mouse_angle < 135) {
                // UNTEN
                if (mob_state.on_ground) return;
                hit_pos = glm::vec2(player_size.x / 2.0f - PLAYER_ATTACK_HITBOX_SIZE.x / 2.0f, -PLAYER_ATTACK_HITBOX_SIZE.y);
                hit_dir = DOWN;
            } else if (mouse_angle >= 135 && mouse_angle < 225) {
                // LINKS
                hit_pos = glm::vec2(-PLAYER_ATTACK_HITBOX_SIZE.x, player_size.y / 2.0f - PLAYER_ATTACK_HITBOX_SIZE.y / 2.0f);
                hit_dir = LEFT;
            } else if (mouse_angle >= 225 && mouse_angle < 315) {
                // OBEN
                hit_pos = glm::vec2(player_size.x / 2.0f - PLAYER_ATTACK_HITBOX_SIZE.x / 2.0f, player_size.y);
                hit_dir = UP;
            } else {
                // RECHTS
                hit_pos = glm::vec2(player_size.x, player_size.y / 2.0f - PLAYER_ATTACK_HITBOX_SIZE.y / 2.0f);
                hit_dir = RIGHT;
            }


            m_registry->keys["mouse_left"] = false;
            attack_state.state = ATTACKING;
            attack_state.attack_box = Rect(hit_pos, PLAYER_ATTACK_HITBOX_SIZE);
            attack_state.attack_dir = hit_dir;
        }
    }
    m_registry->ecs.replace<AttackState>(player_id, attack_state);
}


