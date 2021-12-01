#pragma once
#include "GUI/container.hpp"
#include "GUI/button.hpp"
#include "GUI/rounded_rectangle.hpp"


struct ColonyTool : GUI::Container
{
    ColonyTool()
        : GUI::Container(Container::Orientation::Horizontal)
    {


        padding = 5.0f;
        size_type.y = GUI::Size::FitContent;

        auto color_button = create<GUI::Button>("", [](){});
        color_button->setHeight(30.0f);
        color_button->setWidth(30.0f);
        addItem(color_button);

        auto to_focus_button = create<GUI::Button>("Focus", [](){});
        to_focus_button->setHeight(20.0f);
        to_focus_button->setWidth(40.0f);
        addItem(to_focus_button);

        auto remove_button = create<GUI::Button>("Remove", [](){});
        remove_button->setHeight(20.0f);
        remove_button->setWidth(60.0f);
        addItem(remove_button);
    }

    void render(sf::RenderTarget& target) override
    {
//        auto background = GUI::RoundedRectangle(size, position, 3.0f);
//        background.setFillColor(sf::Color::Red);
//        GUI::Item::draw(target, background);
    }
};
