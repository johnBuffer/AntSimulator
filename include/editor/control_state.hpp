#pragma once
#include <functional>
#include <SFML/Graphics.hpp>
#include "editor/transition.hpp"


struct ControlState
{
    using ViewAction = std::function<void(sf::Vector2f)>;
    using Action     = std::function<void(void)>;
    using DrawAction = std::function<void(sf::RenderTarget&, const ViewportHandler&)>;
    // Actions
    Action     action                       = nullptr;
    ViewAction view_action                  = nullptr;
    Action     view_action_end              = nullptr;
    DrawAction draw_action                  = nullptr;
    std::function<void()> request_edits_off = nullptr;
    // Special commands
    bool                          focus_requested = false;
    trn::Transition<float>        zoom            = 1.0f;
    trn::Transition<sf::Vector2f> focus;
    bool                          updating        = false;
    // Preview callback
    float brush_radius = 0.0f;

    Simulation& simulation;

    explicit
    ControlState(Simulation& sim)
        : simulation(sim)
    {

    }

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

    void executeDrawAction(sf::RenderTarget& target, const ViewportHandler& vp_handler)
    {
        if (draw_action) {
            draw_action(target, vp_handler);
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

    void resetCallbacks()
    {
        view_action     = nullptr;
        draw_action     = nullptr;
        view_action_end = nullptr;
    }
};
