//
// Created by luisb on 18.03.2026.
//


#include <functional>

class State {

public:
    std::function<void()> on_enter;
    std::function<void()> step;
    std::function<void()> on_exit;

};


