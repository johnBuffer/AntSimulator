
#pragma once
#include "item.hpp"
#include "rounded_rectangle.hpp"
#include "../transition.hpp"
#include "text_label.hpp"


namespace GUI
{

using ButtonCallBack = std::function<void()>;

struct DefaultButton : public GUI::Item
{
    ButtonCallBack click_callback = [](){};
    trn::Transition<sf::Vector3f> color;
    
    DefaultButton(sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : GUI::Item(size_, position_)
    {}
    
    DefaultButton(ButtonCallBack callback, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : GUI::Item(size_, position_)
        , click_callback(callback)
    {}

    void onClick(sf::Vector2f, sf::Mouse::Button) override
    {
        click_callback();
    }
};

struct Button : public DefaultButton
{
    float angle_radius = 5.0f;
    SPtr<TextLabel> label;
    sf::Color background_color = sf::Color::White;

    Button(const std::string& text, ButtonCallBack callback)
        : DefaultButton(callback)
    {
        label = create<TextLabel>(text, 14);
        label->catch_event = false;
        addItem(label);
    }
    
    void onSizeChange() override
    {
        label->setSize(size);
    }

    void onPositionChange() override
    {
        label->setPosition({});
    }

    void render(sf::RenderTarget& target) override
    {
        auto background = RoundedRectangle(size, position, angle_radius);
        background.setFillColor(background_color);
        GUI::Item::draw(target, background);
    }
};

}
