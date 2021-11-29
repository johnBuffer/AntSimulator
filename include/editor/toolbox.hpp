#pragma once
#include "GUI/button.hpp"
#include "color_picker.hpp"
#include "GUI/container.hpp"
#include "slider.hpp"

namespace edtr
{

struct Toolbox : public GUI::Container
{
    Toolbox(sf::Vector2f size_, sf::Vector2f position_ = {})
        : GUI::Container(GUI::Container::Orientation::Vertical, size_, position_)
    {}
    
    void render(sf::RenderTarget& target) override
    {
        sf::RectangleShape background(size);
        background.setPosition(position);
        background.setFillColor(sf::Color(100, 100, 100, 120));
        GUI::Item::draw(target, background);
    }
};

}