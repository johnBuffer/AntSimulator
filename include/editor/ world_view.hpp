#pragma once
#include "GUI/item.hpp"
#include "render/renderer.hpp"


namespace edtr
{

struct WorldView : GUI::Item
{
    World& world;
    Renderer renderer;

    explicit
    WorldView(sf::Vector2f size_, World& world_)
        : GUI::Item(size_)
        , world(world_)
    {
    }

    void initializeEventCallbacks() override
    {
        addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e){renderer.vp_handler.wheelZoom(e.mouseWheelScroll.delta);});
        addKeyPressedCallback(sf::Keyboard::R, [&](sfev::CstEv e){renderer.vp_handler.reset();});
    }

    void onClick(sf::Vector2f relative_click_position, sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            renderer.vp_handler.click(relative_click_position);
        }
    }

    void onUnclick(sf::Mouse::Button button) override
    {
        if (button == sf::Mouse::Left) {
            renderer.vp_handler.unclick();
        }
    }

    void onMouseMove(sf::Vector2f new_mouse_position) override
    {
        renderer.vp_handler.setMousePosition(new_mouse_position);
    }

    void render(sf::RenderTarget& target) override
    {
        renderer.render(world, target);
    }
};

}
