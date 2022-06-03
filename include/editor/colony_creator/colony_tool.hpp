#pragma once
#include "editor/GUI/container.hpp"
#include "editor/GUI/button.hpp"
#include "editor/GUI/rounded_rectangle.hpp"
#include "editor/color_picker/color_picker.hpp"
#include "editor/control_state.hpp"
#include "common/color_utils.hpp"
#include "colony_stats.hpp"


struct ColonyTool : GUI::Container
{
    civ::Ref<Colony> colony;
    ControlState& control_state;
    SPtr<GUI::Container> top_zone;
    SPtr<edtr::ColorPicker> color_picker;
    SPtr<ColonyStats> colony_stats;
    sf::Color background_color = {210, 210, 210};

    bool selected = false;
    std::function<void(int8_t)> on_select = [](int8_t){};

    ColonyTool(civ::Ref<Colony>& colony_, ControlState& control_state_)
        : GUI::Container(Container::Orientation::Vertical)
        , colony(colony_)
        , control_state(control_state_)
    {
        // Container configuration
        padding = 5.0f;
        size_type.y = GUI::Size::FitContent;
        // Add the buttons (for color picking, position, focus and remove)
        top_zone = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        top_zone->size_type.y = GUI::Size::FitContent;
        top_zone->padding = 0.0f;
        this->addItem(top_zone);

        auto color_button = create<GUI::Button>("", [this](){
            this->createColorPicker();
        });
        color_button->setHeight(30.0f);
        color_button->setWidth(30.0f);
        top_zone->addItem(color_button, "colony_color_button");

        auto to_focus_button = create<GUI::Button>("Focus", [this](){
            control_state.requestFocus(colony->base.position, 2.0f);
        });
        to_focus_button->setHeight(20.0f);
        to_focus_button->setWidth(40.0f);
        top_zone->addItem(to_focus_button);

        auto set_position_button = create<GUI::Button>("Set Position", [this](){
            control_state.requestEditModeOff();
            // Preview callbacks
            control_state.draw_action = [this](sf::RenderTarget& target, const ViewportHandler& vp_handler) {
                sf::CircleShape c(colony->base.radius);
                c.setOrigin(c.getRadius(), c.getRadius());
                c.setPosition(vp_handler.getMouseWorldPosition());
                const sf::Color colony_color = colony->ants_color;
                c.setFillColor({colony_color.r, colony_color.g, colony_color.b, 100});
                target.draw(c, vp_handler.getRenderState());
            };
            control_state.view_action = [this](sf::Vector2f world_position) {
                const sf::FloatRect bounds = {{0.0f, 0.0f}, control_state.simulation.world.size};
                if (bounds.contains(world_position)) {
                    colony->setPosition(world_position);
                    control_state.draw_action = nullptr;
                    control_state.view_action = nullptr;
                } else {
                    std::cout << "Invalid colony position: outside world" << std::endl;
                }
            };
        });
        set_position_button->setHeight(20.0f);
        set_position_button->setWidth(100.0f);
        top_zone->addItem(set_position_button);

        auto remove_button = create<GUI::Button>("Remove", [](){});
        remove_button->setHeight(20.0f);
        remove_button->setWidth(60.0f);
        remove_button->background_color = sf::Color(250, 200, 200);
        top_zone->addItem(remove_button, "remove");

        auto stats_toggle = create<GUI::NamedToggle>("Stats");
        top_zone->addItem(stats_toggle);
        stats_toggle->onStateChange([this](bool s){
            if (s) {
                GUI::Container::addItem(colony_stats);
            } else {
                GUI::Container::removeItem(colony_stats);
                colony_stats->setPosition({});
            }
        });

        // Create color picker
        color_picker = create<edtr::ColorPicker>();
        color_picker->setHeight(100.0f);
        watch(color_picker, [this](){
            setColor(color_picker->getColor());
        });

        color_picker->setRandomColor();
        colony_stats = create<ColonyStats>(colony, control_state);
    }

    void onClick(sf::Vector2f, sf::Mouse::Button) override
    {
        on_select(colony->id);
    }

    void setColor(sf::Color new_color)
    {
        top_zone->getByName<GUI::Button>("colony_color_button")->background_color = new_color;
        colony->setColor(new_color);
    }

    void createColorPicker()
    {
        if (getByName<edtr::ColorPicker>("color_picker")) {
            return;
        }

        this->addItem(color_picker, "color_picker");
    }

    void onMouseOut() override
    {
        GUI::Container::removeItem(color_picker);
    }

    void render(sf::RenderTarget& target) override
    {
        auto background = GUI::RoundedRectangle(size, position, 5.0f);
        const uint8_t select = selected ? 30 : 0;
        background.setFillColor(sf::Color(background_color.r - select, background_color.g - select, background_color.b - select));
        GUI::Item::draw(target, background);
    }
};
