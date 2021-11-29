#pragma once

#include "GUI/item.hpp"


namespace edtr
{

struct GameView : public GUI::Item
{
    bool pause = false;
    ViewportHandler vp_handler;

    explicit
    GameView(sf::Vector2f size_)
        : GUI::Item(size_)
        , vp_handler(size)
    {
        vp_handler.setZoom(7.5f);
    }
    
    void initializeEventCallbacks() override
    {
        addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e){vp_handler.wheelZoom(e.mouseWheelScroll.delta);});
    }
    
    void onClick(sf::Vector2f relative_click_position, sf::Mouse::Button) override
    {
        vp_handler.click(relative_click_position);
    }
    
    void onUnclick(sf::Mouse::Button) override
    {
        vp_handler.unclick();
    }
    
    void onMouseMove(sf::Vector2f new_mouse_position) override
    {
        vp_handler.setMousePosition(new_mouse_position);
    }
    
    void update() override
    {

    }
    
    void render(sf::RenderTarget& target) override
    {
        GUI::Item::render(target);
    }
};

}
