#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>


struct ControlState
{
    using ViewAction = std::function<void(sf::Vector2f)>;
    using Action = std::function<void(void)>;

    ViewAction view_action = nullptr;

    void executeViewAction(sf::Vector2f mouse_world_position)
    {
        if (view_action) {
            view_action(mouse_world_position);
        }
    }
};
