#pragma once
#include "GUI/item.hpp"
#include "GUI/grid_container.hpp"
#include "GUI/button.hpp"
#include "transition.hpp"


namespace edtr
{

struct ColorSample : public GUI::DefaultButton
{
    sf::Color color = sf::Color::White;
    float angle_radius = 10.0f;
    trn::Transition<float> select_padding = 0.0f;
    trn::Transition<sf::Vector2f> current_size = sf::Vector2f(0.0f, 0.0f);
    trn::Transition<sf::Vector2f> current_position = sf::Vector2f(0.0f, 0.0f);;
    float transition_speed = 3.0f;
    
    ColorSample(sf::Color color_, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : GUI::DefaultButton(size_, position_)
        , color(color_)
    {
        initializeTransitions();
    }
    
    ColorSample(sf::Color color_, GUI::ButtonCallBack callback, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : GUI::DefaultButton(callback, size_, position_)
        , color(color_)
    {
        initializeTransitions();
    }
    
    void initializeTransitions()
    {
        select_padding.setSpeed(transition_speed);
        current_size.setSpeed(transition_speed);
        current_position.setSpeed(transition_speed);
    }
    
    void onPositionChange() override
    {
        current_position = position;
    }
    
    void onSizeChange() override
    {
        current_size = size;
    }
    
    void onMouseIn() override
    {
        select_padding = 10.0f;
    }
    
    void onMouseOut() override
    {
        select_padding = 0.0f;
    }
    
    void render(sf::RenderTarget& target) override
    {
        const sf::Vector2f current_padding = sf::Vector2f(select_padding, select_padding);
        const sf::Vector2f rect_size = current_size.as<sf::Vector2f>();
        const sf::Vector2f inner_size = rect_size - 2.0f * current_padding;
        const sf::Vector2f position_offset = 0.5f * (size - rect_size);
        const sf::Vector2f inner_offset = 0.5f * (size - inner_size);
        const sf::Vector2f pos = current_position;
        
        GUI::RoundedRectangle background(rect_size, pos + position_offset, angle_radius);
        background.setFillColor(sf::Color::White);
        GUI::RoundedRectangle color_rect(inner_size, pos + inner_offset, angle_radius - select_padding);
        color_rect.setFillColor(color);
        draw(target, background);
        draw(target, color_rect);
    }
};


struct ColorButton : public GUI::DefaultButton
{
    SPtr<GUI::Container> container;
    SPtr<ColorSample>    sample;
    SPtr<GUI::TextLabel> label;
    sf::Clock clock;
    
    ColorButton(const std::string& text, GUI::ButtonCallBack callback)
        : GUI::DefaultButton(callback)
    {
        container = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        sample = create<ColorSample>(sf::Color::Red);
        label = create<GUI::TextLabel>(text, 32);
        
        container->padding = 10.0f;
        container->spacing = 0.0f;
        container->ignoreEvents();
        
        container->addItem(sample);
        container->addItem(label);
        
        addItem(container);
    }
    
    void onSizeChange() override
    {
        container->setSize(size);
        sample->setWidth(sample->size.y);
    }

    void render(sf::RenderTarget& target) override
    {
        const float angle_radius = 15.0f;
        GUI::Item::draw(target, GUI::RoundedRectangle(size, position, angle_radius));
    }
    
    void setColor(sf::Color new_color)
    {
        sample->color = new_color;
    }
};


struct ColorSaver : public GUI::Item
{
    sf::Color loaded_color;
    std::shared_ptr<GUI::GridContainer> grid;
    std::shared_ptr<ColorButton> current_color_add;
    
    ColorSaver(sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : GUI::Item(size_, position_)
    {
        padding = 0.0f;
        grid = create<GUI::GridContainer>(size_, sf::Vector2i(5, 5));
        current_color_add = create<ColorButton>("Save", [this](){
            addColor(current_color_add->sample->color);
        });
        // Add items
        addItem(grid);
    }
    
    void onSizeChange() override
    {
        setHeight(size.x);
        grid->setSize(size);
    }
    
    void addColor(sf::Color color)
    {
        grid->addItem(create<ColorSample>(color, [this, color](){
            this->loaded_color = color;
            this->notifyChanged();
        }));
        reorderGrid();
    }
    
    void reorderGrid()
    {
        GUI::ItemPtr last_item = grid->sub_items.back();
        grid->sub_items.pop_back();
        grid->sub_items.insert(grid->sub_items.begin(), last_item);
        if (grid->sub_items.size() > grid->getMaxItemsCount()) {
            grid->sub_items.resize(grid->getMaxItemsCount());
        }
        grid->updateItems();
    }
};

}
