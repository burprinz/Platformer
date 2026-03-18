//
// Created by luisb on 14.03.2026.
//

#include "animation.h"

AnimationSystem AnimationSystem::init(Window *window, Registry *registry) noexcept {
    AnimationSystem self;
    self.m_window = window;
    self.m_registry = registry;

    return self;
}

void AnimationSystem::deinit() noexcept {

}

void AnimationSystem::reset() noexcept {

}

void AnimationSystem::step(const float delta) noexcept {
    (void) delta;
}



