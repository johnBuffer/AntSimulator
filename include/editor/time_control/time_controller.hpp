#pragma once
#include "editor/tool_selector.hpp"
#include "editor/GUI/toggle.hpp"


namespace edtr
{

struct TimeController : public GUI::NamedContainer
{
    enum class State
    {
        Play,
        Pause,
    };

    State current_state = State::Pause;

    SPtr<ToolOption> tool_pause;
    SPtr<ToolOption> tool_play;
    SPtr<GUI::NamedToggle> tool_speed;

    TimeController()
            : GUI::NamedContainer("Time Control", Container::Orientation::Horizontal)
            , current_state(State::Pause)
    {
        size_type.x       = GUI::Size::FitContent;
        size_type.y       = GUI::Size::FitContent;
        root->size_type.x = GUI::Size::FitContent;
        root->size_type.y = GUI::Size::FitContent;

        tool_pause = create<ToolOption>("PAUSE", [this](){
            current_state = State::Pause;
            select(State::Pause);
        });
        tool_pause->color = {255, 255, 230};
        tool_pause->setWidth(70.0f);

        tool_play = create<ToolOption>("PLAY", [this](){
            current_state = State::Play;
            select(State::Play);
        });
        tool_play->color = {230, 255, 230};
        tool_play->setWidth(70.0f);

        tool_speed = create<GUI::NamedToggle>("Full Speed");
        tool_speed->setWidth(70.0f);
        watch(tool_speed, [this]{
            notifyChanged();
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
        }
        notifyChanged();
    }
};

}
