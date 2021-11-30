#pragma once
#include "GUI/container.hpp"
#include "GUI/button.hpp"
#include "GUI/rounded_rectangle.hpp"


struct ColonyTool : GUI::Container
{
    SPtr<GUI::Container> bottom;

    ColonyTool()
        : GUI::Container(Container::Orientation::Vertical)
    {
        auto top = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        top->size_type.y = GUI::Size::FitContent;
        GUI::Container::addItem(top);

        padding = 5.0f;
        size_type.y = GUI::Size::FitContent;

        auto label = create<GUI::TextLabel>("Colony", 14);
        top->addItem(label);
        auto color_button = create<GUI::Button>("", [](){});
        color_button->setHeight(10.0f);
        color_button->setWidth(20.0f);
        top->addItem(color_button);

        bottom = create<GUI::Container>(GUI::Container::Orientation::Horizontal);
        bottom->size_type.y = GUI::Size::FitContent;
        auto to_focus_button = create<GUI::Button>("Focus", [](){});
        to_focus_button->setHeight(20.0f);
        to_focus_button->setWidth(40.0f);
        bottom->addItem(to_focus_button);

        auto remove_button = create<GUI::Button>("Remove", [](){});
        remove_button->setHeight(20.0f);
        remove_button->setWidth(60.0f);
        bottom->addItem(remove_button);
    }

    void onMouseIn() override
    {
        GUI::Container::addItem(bottom);
    }

    void onMouseOut() override
    {
        GUI::Container::removeItem(bottom);
    }

    void render(sf::RenderTarget& target) override
    {
//        auto background = GUI::RoundedRectangle(size, position, 3.0f);
//        background.setFillColor(sf::Color::Red);
//        GUI::Item::draw(target, background);
    }
};
