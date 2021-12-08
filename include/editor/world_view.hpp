#pragma once
#include "GUI/item.hpp"
#include "render/renderer.hpp"
#include "editor/control_state.hpp"
#include "editor/time_control/time_controller.hpp"


namespace edtr
{

struct WorldView : GUI::Item
{
    Simulation&           simulation;
    ControlState&         control_state;
    TimeController::State current_time_state  = TimeController::State::Pause;
    bool                  action_button_click = false;

    explicit
    WorldView(sf::Vector2f size_, Simulation& simulation_, ControlState& control_state_)
        : GUI::Item(size_)
        , simulation(simulation_)
        , control_state(control_state_)
    {
        simulation.renderer.vp_handler.reset();
        control_state.focus.setValueInstant(simulation.renderer.vp_handler.state.offset);
        control_state.zoom.setValueInstant(simulation.renderer.vp_handler.state.zoom);
    }

    void initializeEventCallbacks() override
    {
        addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e){
            simulation.renderer.vp_handler.wheelZoom(e.mouseWheelScroll.delta);
            control_state.focus_requested = false;
            control_state.zoom.setValueInstant(simulation.renderer.vp_handler.state.zoom);
        });
        addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv){
            control_state.focus_requested = true;
            control_state.zoom = 1.0f;
            control_state.focus = simulation.renderer.vp_handler.state.center;
        });
    }

    void onClick(sf::Vector2f relative_click_position, sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            control_state.focus_requested = false;
            simulation.renderer.vp_handler.click(relative_click_position);
        } else if (button == sf::Mouse::Right) {
            action_button_click = true;
            control_state.executeViewAction(simulation.renderer.vp_handler.getMouseWorldPosition());
        }
    }

    void onUnclick(sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            simulation.renderer.vp_handler.unclick();
            control_state.focus_requested = false;
            control_state.focus.setValueInstant(simulation.renderer.vp_handler.state.offset);
        } else if (button == sf::Mouse::Button::Right) {
            action_button_click = false;
            control_state.executeViewActionEnd();
        }
    }

    void onMouseMove(sf::Vector2f new_mouse_position) override
    {
        simulation.renderer.vp_handler.setMousePosition(new_mouse_position);
        if (action_button_click) {
            control_state.executeViewAction(simulation.renderer.vp_handler.getMouseWorldPosition());
        }
    }

    void render(sf::RenderTarget& target) override
    {
        if (control_state.focus_requested) {
            simulation.renderer.vp_handler.setFocus(control_state.focus);
            simulation.renderer.vp_handler.setZoom(control_state.zoom);
        }
        simulation.render(target);
        if (control_state.show_brush_preview) {
            const int32_t cell_size = simulation.world.map.cell_size;
            const float side_size = (2.0f * control_state.brush_radius + 1) * cell_size;
            sf::RectangleShape brush_preview({side_size, side_size});
            brush_preview.setFillColor(sf::Color(100, 100, 100, 100));
            brush_preview.setOrigin(side_size * 0.5f, side_size * 0.5f);
            const sf::Vector2f current_position = simulation.renderer.vp_handler.getMouseWorldPosition();
            brush_preview.setPosition(to<float>(int(current_position.x / to<float>(cell_size)) * cell_size) + 2.0f,
                                      to<float>(int(current_position.y / to<float>(cell_size)) * cell_size) + 2.0f);
            target.draw(brush_preview, simulation.renderer.vp_handler.getRenderState());
        }
    }

    void update() override
    {
        const float dt = 0.016f;
        if (current_time_state == TimeController::State::Play) {
            simulation.update(dt);
        }
    }
};

}
