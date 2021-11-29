#pragma once
#include "GUI/button.hpp"


namespace edtr
{

struct SetColorButton : GUI::DefaultButton
{
    sf::Color color;
    float angle_radius = 10.0f;
    SPtr<GUI::TextLabel> label;

    SetColorButton(const std::string& text, GUI::ButtonCallBack callback)
        : DefaultButton(callback)
    {
        label = create<GUI::TextLabel>(text, 32);
        label->catch_event = false;
        addItem(label);
    }
    
    void onSizeChange() override
    {
        label->setSize(size);
    }

    void render(sf::RenderTarget& target) override
    {
        GUI::RoundedRectangle background(size, position, angle_radius);
        GUI::Item::draw(target, background);
    }
};

}
