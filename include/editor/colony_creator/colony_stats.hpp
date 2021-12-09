#pragma once
#include "editor/GUI/named_container.hpp"
#include "common/graph.hpp"


struct ColonyChart : public GUI::Item
{
    civ::Ref<Colony> colony;
    ControlState& control_state;
    Graphic chart;
    uint32_t frame_count = 0;
    SPtr<Item> chart_zone;

    explicit
    ColonyChart(civ::Ref<Colony> colony_, ControlState& control_state_)
        : chart(1000, {}, {})
        , colony(colony_)
        , control_state(control_state_)
    {
        size_type.x = GUI::Size::Auto;
        size_type.y = GUI::Size::Auto;
    }

    void onSizeChange() override
    {
        chart.width  = size.x;
        chart.height = size.y;
    }

    void onPositionChange() override
    {
        chart.x = position.x;
        chart.y = position.y;
    }

    void update() override
    {
        if (control_state.updating) {
            if (frame_count % 10 == 0) {
                chart.addValue(to<float>(colony->ants.size()));
            }
            ++frame_count;
        }
    }

    void render(sf::RenderTarget& target) override
    {
        chart.color = colony->ants_color;
        GUI::Item::draw(target, chart);
    }
};


struct ColonyStats : public GUI::NamedContainer
{
    SPtr<GUI::TextLabel> population_label;
    civ::Ref<Colony> colony;

    ColonyStats(civ::Ref<Colony> colony_, ControlState& control_state)
        : GUI::NamedContainer("Population")
        , colony(colony_)
    {
        header->addItem(create<GUI::EmptyItem>());

        population_label = create<GUI::TextLabel>("", 14);
        header->addItem(population_label);
        root->size_type = {GUI::Size::Auto, GUI::Size::Auto};
        setHeight(100.0f);
        GUI::NamedContainer::addItem(create<ColonyChart>(colony, control_state));
    }

    void update() override
    {
        population_label->setText(toStr(colony->ants.size()));
    }

//    void render(sf::RenderTarget& target) override
//    {
//
//    }
};
