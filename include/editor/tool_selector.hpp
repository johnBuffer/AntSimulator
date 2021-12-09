#pragma once
#include "GUI/container.hpp"
#include "GUI/button.hpp"
#include "control_state.hpp"
#include "simulation/simulation.hpp"
#include <future>


namespace edtr
{


struct ToolOption : public GUI::Button
{
    trn::Transition<float> select_padding = 0.0f;
    sf::Color color = sf::Color::White;
    
    ToolOption(const std::string& text, GUI::ButtonCallBack callback)
        : GUI::Button(text, callback)
    {
        select_padding.setSpeed(3.0f);
    }
    
    void select()
    {
        select_padding = 5.0f;
    }
    
    void reset()
    {
        select_padding = 0.0f;
    }
    
    void render(sf::RenderTarget& target) override
    {
        const float tool_angle_radius = 5.0f;
        const sf::Vector2f current_padding = sf::Vector2f(select_padding, select_padding);
        const sf::Vector2f inner_size = size - 2.0f * current_padding;
        const sf::Vector2f inner_offset = 0.5f * (size - inner_size);
        
        GUI::RoundedRectangle background(size, position, tool_angle_radius);
        background.setFillColor(sf::Color(200, 200, 200));
        GUI::RoundedRectangle color_rect(inner_size, position + inner_offset, tool_angle_radius - select_padding);
        color_rect.setFillColor(color);
        draw(target, background);
        draw(target, color_rect);
    }
};


struct ToolSelector : public GUI::NamedContainer
{
    enum class Tool
    {
        BrushWall,
        BrushFood,
        BrushDelete
    };

    Tool current_tool;

    SPtr<ToolOption> tool_wall;
    SPtr<ToolOption> tool_food;
    SPtr<ToolOption> tool_erase;

    bool          edit_mode = false;
    Simulation&   simulation;
    ControlState& control_state;

    int32_t brush_size = 3;

    explicit
    ToolSelector(ControlState& control_state_, Simulation& simulation_)
        : GUI::NamedContainer("Brushes", Container::Orientation::Horizontal)
        , current_tool(Tool::BrushWall)
        , control_state(control_state_)
        , simulation(simulation_)
    {
        padding = 5.0f;

        root->setHeight(30.0f, GUI::Size::Fixed);
        tool_wall = create<ToolOption>("Wall", [this](){
            current_tool = Tool::BrushWall;
            select(tool_wall);
        });
        tool_food = create<ToolOption>("Food", [this](){
            current_tool = Tool::BrushFood;
            select(tool_food);
        });
        tool_erase = create<ToolOption>("Erase", [this](){
            current_tool = Tool::BrushDelete;
            select(tool_erase);
        });
        tool_food->color = sf::Color(200, 255, 200);
        tool_erase->color = sf::Color(255, 200, 200);
        // Add items
        addItem(tool_wall);
        addItem(tool_food);
        addItem(tool_erase);
        // Default selection
        select(tool_wall);
    }
    
    void reset() const
    {
        tool_wall->reset();
        tool_food->reset();
        tool_erase->reset();
    }
    
    void select(const SPtr<ToolOption>& option)
    {
        reset();
        if (option) {
            option->select();
            setCallback();
        }
        notifyChanged();
    }

    template<typename TCallback>
    void applyBrush(sf::Vector2f mouse_position, TCallback&& callback)
    {
        const auto x = to<int32_t>(mouse_position.x) / simulation.world.map.cell_size;
        const auto y = to<int32_t>(mouse_position.y) / simulation.world.map.cell_size;

        const int32_t min_x = std::max(1, x - brush_size);
        const int32_t max_x = std::min(to<int32_t>(simulation.world.map.width - 1), x + brush_size + 1);
        const int32_t min_y = std::max(1, y - brush_size);
        const int32_t max_y = std::min(to<int32_t>(simulation.world.map.height - 1), y + brush_size + 1);

        for (int32_t px(min_x); px < max_x; ++px) {
            for (int32_t py(min_y); py < max_y; ++py) {
                callback(px, py);
            }
        }
    }

    void setCallback()
    {
        if (edit_mode) {
            // Edit callbacks
            switch (current_tool) {
                case Tool::BrushWall:
                    control_state.view_action = [this](sf::Vector2f mouse_position) {
                        applyBrush(mouse_position, [this](int32_t x, int32_t y){
                            simulation.world.addWall(sf::Vector2i{x, y});
                        });
                    };
                    control_state.view_action_end = [this]() {
                        simulation.distance_field_builder.requestUpdate();
                    };
                    break;
                case Tool::BrushFood:
                    control_state.view_action = [this](sf::Vector2f mouse_position) {
                        applyBrush(mouse_position, [this](int32_t x, int32_t y){
                            simulation.world.addFoodAt(sf::Vector2i{x, y}, 2);
                        });
                    };
                    break;
                case Tool::BrushDelete:
                    control_state.view_action = [this](sf::Vector2f mouse_position) {
                        applyBrush(mouse_position, [this](int32_t x, int32_t y){
                            simulation.world.map.clearCell(sf::Vector2i{x, y});
                        });
                        simulation.distance_field_builder.requestUpdate();
                    };
                    control_state.view_action_end = [this]() {
                        simulation.distance_field_builder.requestUpdate();
                    };
                    break;
            }
            // Preview callbacks
            control_state.draw_action = [this](sf::RenderTarget& target, const ViewportHandler& vp_handler) {
                const int32_t cell_size = simulation.world.map.cell_size;
                const float side_size = (2.0f * control_state.brush_radius + 1) * to<float>(cell_size);
                sf::RectangleShape brush_preview({side_size, side_size});
                brush_preview.setFillColor(sf::Color(100, 100, 100, 100));
                brush_preview.setOrigin(side_size * 0.5f, side_size * 0.5f);
                const sf::Vector2f current_position = vp_handler.getMouseWorldPosition();
                brush_preview.setPosition(to<float>(int(current_position.x / to<float>(cell_size)) * cell_size) + 2.0f,
                                          to<float>(int(current_position.y / to<float>(cell_size)) * cell_size) + 2.0f);
                target.draw(brush_preview, vp_handler.getRenderState());
            };
        }
    }

    void setEditMode(bool b)
    {
        edit_mode = b;
        if (b) {
            setCallback();
        } else {
            resetCallback();
        }
    }

    void resetCallback()
    {
        control_state.view_action     = nullptr;
        control_state.view_action_end = nullptr;
        control_state.draw_action     = nullptr;
    }
};

}

