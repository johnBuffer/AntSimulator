#pragma once
#include <iostream>
#include <render/viewport_handler.hpp>
#include "object.hpp"
#include "selection.hpp"


namespace edtr
{

struct Controller
{
    Object& object;
    bool selecting;
    bool add_selection;
    Selection selection;
    
    Controller(Object& object_)
        : object(object_)
        , selecting(false)
        , add_selection(false)
    {
    }
    
    void updateSelection(sf::Vector2f mouse_position)
    {
        if (selecting) {
            selection.setPoint2(IVec2(mouse_position.x, mouse_position.y));
            object.map([&](IVec2 coords, int32_t, Atom& atom) {
                atom.hover = (coords.x >= selection.start.x && coords.x <= selection.end.x &&
                              coords.y >= selection.start.y && coords.y <= selection.end.y);
            });
        }
    }
    
    void resetSelection()
    {
        add_selection = false;
        object.map([&](IVec2 coords, int32_t, Atom& atom) {
            atom.selected = false;
            atom.hover = false;
        });
    }
    
    void startSelection(sf::Vector2f mouse_position)
    {
        selecting = true;
        if (!add_selection) {
            resetSelection();
        }
        selection.setPoint1(IVec2(mouse_position.x, mouse_position.y));
        selection.setPoint2(IVec2(mouse_position.x, mouse_position.y));
    }
    
    void stopSelection()
    {
        selecting = false;
        object.map([&](IVec2 coords, int32_t, Atom& atom) {
            if (atom.hover) {
                atom.selected = true;
                atom.hover = false;
            }
        });
    }
};

}
