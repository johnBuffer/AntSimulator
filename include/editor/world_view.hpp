#pragma once
#include "GUI/item.hpp"
#include "render/renderer.hpp"
#include "editor/control_state.hpp"


namespace edtr
{

struct WorldView : GUI::Item
{
    Simulation&   simulation;
    ControlState& control_state;

    explicit
    WorldView(sf::Vector2f size_, Simulation& simulation_, ControlState& control_state_)
        : GUI::Item(size_)
        , simulation(simulation_)
        , control_state(control_state_)
    {
    }

    void initializeEventCallbacks() override
    {
        addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e){simulation.renderer.vp_handler.wheelZoom(e.mouseWheelScroll.delta);});
        addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv e){simulation.renderer.vp_handler.reset();});
    }

    void onClick(sf::Vector2f relative_click_position, sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            simulation.renderer.vp_handler.click(relative_click_position);
        } else if (button == sf::Mouse::Right) {
            control_state.executeViewAction(simulation.renderer.vp_handler.getMouseWorldPosition());
        }
    }

    void onUnclick(sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            simulation.renderer.vp_handler.unclick();
        }
    }

    void onMouseMove(sf::Vector2f new_mouse_position) override
    {
        simulation.renderer.vp_handler.setMousePosition(new_mouse_position);
    }

    void render(sf::RenderTarget& target) override
    {
        simulation.render(target);
    }
};

}
