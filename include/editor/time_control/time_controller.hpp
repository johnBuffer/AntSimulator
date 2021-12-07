#pragma once
#include "editor/tool_selector.hpp"

namespace edtr
{

struct TimeController : public GUI::NamedContainer
{
    enum class State
    {
        Play,
        Pause,
        Speed
    };

    State current_state = State::Pause;

    SPtr<ToolOption> tool_pause;
    SPtr<ToolOption> tool_play;
    SPtr<ToolOption> tool_speed;

    TimeController()
            : GUI::NamedContainer("Time Control", Container::Orientation::Horizontal)
            , current_state(State::Pause)
    {
        setWidth(180.0f);
        
        root->setHeight(30.0f, GUI::Size::Fixed);
        tool_pause = create<ToolOption>("Pause", [this](){
            current_state = State::Pause;
            select(State::Pause);
        });
        tool_play = create<ToolOption>("Play", [this](){
            current_state = State::Play;
            select(State::Play);
        });
        tool_speed = create<ToolOption>("Speed", [this](){
            current_state = State::Speed;
            select(State::Speed);
        });

        // Add items
        addItem(tool_pause);
        addItem(tool_play);
        addItem(tool_speed);
        // Default selection
        select(State::Pause);
    }

    void reset()
    {
        tool_pause->reset();
        tool_play->reset();
        tool_speed->reset();
    }

    void select(State option)
    {
        reset();
        current_state = option;
        switch (option) {
            case State::Pause:
                tool_pause->select();
                break;
            case State::Play:
                tool_play->select();
                break;
            case State::Speed:
                tool_speed->select();
                break;
        }
        notifyChanged();
    }
};

}
