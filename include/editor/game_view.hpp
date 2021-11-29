#pragma once

#include <engine.hpp>
#include <render/object_renderer.hpp>
#include "GUI/item.hpp"


namespace edtr
{

struct GameView : public GUI::Item
{
    bool pause = false;
    Engine engine;
    ViewportHandler vp_handler;
    bool moving_object;
    uint64_t last_object;
    
    GameView(sf::Vector2f size_)
        : GUI::Item(size_)
        , engine(512, 512)
        , vp_handler(size)
    {
        PhysicObject ground(512, 4, 0.0f, 0.0f);
        ground.setBottomStatic();
        ground.position = Vec2(256.0f, 508.0f);
        engine.addEntity(ground);
        
        vp_handler.setFocus(Vec2(256.0f, 410.0f));
        vp_handler.setZoom(7.5f);
    }
    
    void resetObjects()
    {
        
    }
    
    void addEntity(const PhysicObject& e)
    {
        pause = true;
        moving_object = true;
        last_object = engine.addEntity(e);
        engine.getRef(last_object).physic_object->need_break_check = true;
        engine.checkForParts();
    }
    
    void initializeEventCallbacks() override
    {
        addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e){vp_handler.wheelZoom(e.mouseWheelScroll.delta);});
        addKeyPressedCallback(sf::Keyboard::Space, [&](sfev::CstEv){play_stop();});
    }
    
    void play_stop()
    {
        if (pause) {
            moving_object = false;
        }
        pause = !pause;
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
        if (moving_object) {
            const sf::Vector2f world_pos = vp_handler.getMouseWorldPosition();
            engine.getRef(last_object).physic_object->position = Vec2(world_pos.x, world_pos.y);
        }
    }
    
    void update() override
    {
        constexpr float dt = 1.0f / 60.0f;
        if (!pause) {
            engine.update(dt);
        }
    }
    
    void render(sf::RenderTarget& target) override
    {
        engine.render(target, vp_handler.getRenderState());
        GUI::Item::render(target);
    }
};

}
