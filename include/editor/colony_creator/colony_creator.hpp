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
    int32_t       last_selected = -1;

    explicit
    ColonyCreator(Simulation& sim, ControlState& control_state_)
        : GUI::NamedContainer("Colonies", Container::Orientation::Vertical)
        , simulation(sim)
        , control_state(control_state_)
    {
        root->size_type.y = GUI::Size::FitContent;
        auto add_button = create<GUI::Button>("Add", [this](){
            this->createColony();
        });
        add_button->setWidth(32.0f, GUI::Size::Fixed);
        add_button->setHeight(20.0f, GUI::Size::Fixed);
        header->addItem(create<GUI::EmptyItem>());
        header->addItem(add_button);
    }

    void createColony()
    {
        if (this->colonies_count < Conf::MAX_COLONIES_COUNT) {
            auto new_colony = simulation.createColony(50.0f, 50.0f);
            auto colony_tool = create<ColonyTool>(new_colony, control_state);
            colony_tool->on_select = [this](int8_t id){
                select(id);
            };
            // Add the new item to this
            this->addItem(colony_tool);
            ++this->colonies_count;

            // Set the correct callback for the remove button
            colony_tool->top_zone->getByName<GUI::Button>("remove")->click_callback = [this, colony_tool](){
                simulation.removeColony(colony_tool->colony->id);
                this->removeItem(colony_tool);
                --this->colonies_count;
            };
        }
    }

    void select(int32_t id)
    {
        int32_t selected = -1;
        if (id != last_selected) {
            selected = id;
        }
        last_selected = selected;
        this->simulation.world.renderer.selected_colony = selected;

        for (const auto& item : root->sub_items) {
            auto tool = std::dynamic_pointer_cast<ColonyTool>(item);
            if (tool) {
                tool->selected = tool->colony->id == selected;
            }
        }
    }
};

}

