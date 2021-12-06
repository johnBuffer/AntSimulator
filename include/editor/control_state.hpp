#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include "editor/transition.hpp"


struct ControlState
{
    using ViewAction = std::function<void(sf::Vector2f)>;
    using Action     = std::function<void(void)>;
    // Actions
    Action     action      = nullptr;
    ViewAction view_action = nullptr;
    // Special commands
    bool focus_requested = false;
    trn::Transition<sf::Vector2f> focus;
    trn::Transition<float> zoom = 1.0f;

    ControlState() = default;

    void executeViewAction(sf::Vector2f mouse_world_position)
    {
        if (view_action) {
            view_action(mouse_world_position);
        }
    }

    void executeAction(sf::Vector2f)
    {
        if (action) {
            action();
        }
    }

    void requestFocus(sf::Vector2f position, float zoom_level = 1.0f)
    {
        focus_requested = true;
        focus           = position;
        zoom            = zoom_level;
    }
};
