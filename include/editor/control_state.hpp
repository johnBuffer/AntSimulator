#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include "editor/transition.hpp"


struct ControlState
{
    using ViewAction = std::function<void(sf::Vector2f)>;
    using Action     = std::function<void(void)>;
    // Actions
    Action     action          = nullptr;
    ViewAction view_action     = nullptr;
    Action     view_action_end = nullptr;
    // Special commands
    bool focus_requested                = false;
    trn::Transition<sf::Vector2f> focus;
    trn::Transition<float>        zoom  = 1.0f;
    // Preview callback
    bool show_brush_preview = false;
    float brush_radius      = 0.0f;

    std::function<void()> request_edits_off = nullptr;

    ControlState() = default;

    void executeViewAction(sf::Vector2f mouse_world_position)
    {
        if (view_action) {
            view_action(mouse_world_position);
        }
    }

    void executeViewActionEnd()
    {
        if (view_action_end) {
            view_action_end();
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

    void requestEditModeOff()
    {
        if (request_edits_off) {
            request_edits_off();
        }
    }
};
