#pragma once
#include "editor/GUI/named_container.hpp"
#include "editor/GUI/utils.hpp"
#include "editor/control_state.hpp"
#include "editor/GUI/toggle.hpp"


struct DisplayOption : public GUI::NamedContainer
{
    ControlState& control_state;

    bool draw_ants    = false;
    bool draw_markers = false;
    bool draw_density = false;

    explicit
    DisplayOption(ControlState& control_state_)
        : GUI::NamedContainer("Display Options", GUI::Container::Orientation::Horizontal)
        , control_state(control_state_)
    {
        size_type.y = GUI::Size::FitContent;
        auto ants = create<GUI::NamedToggle>("Draw Ants");
        watch(ants, [this, ants](){
            draw_ants = ants->getState();
            notifyChanged();
        });
        ants->setState(true);
        GUI::NamedContainer::addItem(ants);

        auto markers = create<GUI::NamedToggle>("Draw Markers");
        GUI::NamedContainer::addItem(markers);
        auto density = create<GUI::NamedToggle>("Draw Density");
        GUI::NamedContainer::addItem(density);

        watch(markers, [this, markers, density](){
            if (markers->getState()) {
                density->setState(false);
            }
            draw_markers = markers->getState();
            draw_density = density->getState();
            notifyChanged();
        });

        watch(density, [this, markers, density](){
            if (density->getState()) {
                markers->setState(false);
            }
            draw_markers = markers->getState();
            draw_density = density->getState();
            notifyChanged();
        });
    }
};

