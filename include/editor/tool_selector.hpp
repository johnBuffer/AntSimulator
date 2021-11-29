#pragma once
#include "GUI/container.hpp"
#include "GUI/button.hpp"


namespace edtr
{


struct ToolOption : public GUI::Button
{
    trn::Transition<float> select_padding = 0.0f;
    
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
        const float angle_radius = 10.0f;
        const sf::Vector2f current_padding = sf::Vector2f(select_padding, select_padding);
        const sf::Vector2f inner_size = size - 2.0f * current_padding;
        const sf::Vector2f inner_offset = 0.5f * (size - inner_size);
        
        GUI::RoundedRectangle background(size, position, angle_radius);
        background.setFillColor(sf::Color(200, 200, 200));
        GUI::RoundedRectangle color_rect(inner_size, position + inner_offset, angle_radius - select_padding);
        color_rect.setFillColor(sf::Color::White);
        draw(target, background);
        draw(target, color_rect);
    }
};


struct ToolSelector : public GUI::Container
{
    enum class Tool
    {
        BrushCreate,
        BrushDelete,
        BrushColor
    };
    
    Tool current_tool;
    
    SPtr<ToolOption> tool_create;
    SPtr<ToolOption> tool_delete;
    SPtr<ToolOption> tool_color;
    
    ToolSelector()
        : GUI::Container(Container::Orientation::Horizontal)
        , current_tool(Tool::BrushCreate)
    {
        padding = 0.0f;
        setHeight(70.0f);
        tool_create = create<ToolOption>("Create", [this](){
            current_tool = Tool::BrushCreate;
            select(tool_create);
        });
        tool_delete = create<ToolOption>("Delete", [this](){
            current_tool = Tool::BrushDelete;
            select(tool_delete);
        });
        tool_color = create<ToolOption>("Color", [this](){
            current_tool = Tool::BrushColor;
            select(tool_color);
        });
        // Add items
        addItem(tool_create);
        addItem(tool_delete);
        addItem(tool_color);
        // Default selection
        select(tool_create);
    }
    
    void reset()
    {
        tool_create->reset();
        tool_delete->reset();
        tool_color->reset();
    }
    
    void select(SPtr<ToolOption> option)
    {
        reset();
        option->select();
        notifyChanged();
    }
};

}

