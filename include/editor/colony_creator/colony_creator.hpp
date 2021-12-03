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
                auto colony_tool = create<ColonyTool>();

                this->addItem(colony_tool);
                ++this->colonies_count;

                colony_tool->top_zone->getByName<GUI::Button>("remove")->click_callback = [this, colony_tool](){
                    this->removeItem(colony_tool);
                    --this->colonies_count;
                };
            }
        });
        add_button->setWidth(32.0f, GUI::Size::Fixed);
        add_button->setHeight(20.0f, GUI::Size::Fixed);
        header->addItem(add_button);
    }
};

}

