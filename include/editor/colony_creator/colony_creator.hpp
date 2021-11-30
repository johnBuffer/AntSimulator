#pragma once
#include "editor/GUI/container.hpp"
#include "editor/GUI/button.hpp"
#include "colony_tool.hpp"


namespace edtr
{

struct ColonyCreator : public GUI::NamedContainer
{
    uint32_t colonies_count = 0;

    ColonyCreator()
        : GUI::NamedContainer("Colonies", Container::Orientation::Vertical)
    {
        padding = 5.0f;
        root->size_type.y = GUI::Size::FitContent;
        auto add_button = create<GUI::Button>("Add", [this](){
            if (this->colonies_count < 8) {
                this->addItem(create<ColonyTool>());
                ++this->colonies_count;
            }
        });
        add_button->setWidth(32.0f, GUI::Size::Fixed);
        add_button->setHeight(24.0f, GUI::Size::Fixed);
        header->addItem(add_button);
    }
};

}

