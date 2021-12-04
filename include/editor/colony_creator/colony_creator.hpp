#pragma once
#include "editor/GUI/container.hpp"
#include "editor/GUI/button.hpp"
#include "colony_tool.hpp"
#include "simulation/simulation.hpp"


namespace edtr
{

struct ColonyCreator : public GUI::NamedContainer
{
    Simulation&   simulation;
    ControlState& control_state;
    uint32_t      colonies_count = 0;

    explicit
    ColonyCreator(Simulation& sim, ControlState& control_state_)
        : GUI::NamedContainer("Colonies", Container::Orientation::Vertical)
        , simulation(sim)
        , control_state(control_state_)
    {
        padding = 5.0f;
        root->size_type.y = GUI::Size::FitContent;
        auto add_button = create<GUI::Button>("Add", [this](){
            this->createColony();
        });
        add_button->setWidth(32.0f, GUI::Size::Fixed);
        add_button->setHeight(20.0f, GUI::Size::Fixed);
        header->addItem(add_button);
    }

    void createColony()
    {
        if (this->colonies_count < Conf::MAX_COLONIES_COUNT) {
            Colony& new_colony = simulation.createColony(50.0f, 50.0f);
            auto colony_tool = create<ColonyTool>(new_colony, control_state);

            // Add the new item to this
            this->addItem(colony_tool);
            ++this->colonies_count;

            // Set the correct callback for the remove button
            colony_tool->top_zone->getByName<GUI::Button>("remove")->click_callback = [this, colony_tool](){
                this->removeItem(colony_tool);
                --this->colonies_count;
            };
        }
    }
};

}

