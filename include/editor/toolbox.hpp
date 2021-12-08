#pragma once
#include "GUI/button.hpp"
#include "editor/color_picker/color_picker.hpp"
#include "editor/GUI/named_container.hpp"
#include "editor/GUI/toggle.hpp"
#include "slider.hpp"
#include "common/dynamic_blur.hpp"


namespace edtr
{

struct Toolbox : public GUI::NamedContainer
{
    explicit
    Toolbox(sf::Vector2f size_, sf::Vector2f position_ = {})
        : GUI::NamedContainer("Toolbox", GUI::Container::Orientation::Vertical)
    {
        header->addItem(create<GUI::EmptyItem>());
        auto tools_toggle = create<GUI::Toggle>();
        tools_toggle->color_on = {240, 180, 0};
        tools_toggle->setState(true);
        watch(tools_toggle, [this, tools_toggle]{
           if (tools_toggle->state) {
               showRoot();
           } else {
               hideRoot();
           }
        });
        header->addItem(tools_toggle);

        setPosition(position_);
        background_intensity = 180;
        setWidth(size_.x);
    }
    
//    void render(sf::RenderTarget& target) override
//    {
//        sf::RectangleShape background(size);
//        background.setPosition(position);
//        background.setFillColor(sf::Color(100, 100, 100, 200));
//        GUI::Item::draw(target, background);
//    }
};

}
