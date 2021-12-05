#pragma once
#include "GUI/item.hpp"
#include "utils.hpp"
#include <common/event_manager.hpp>
#include <vector>


namespace GUI
{

struct Scene
{
    using Ptr = std::shared_ptr<Scene>;
    
    sf::RenderWindow&  window;
    sfev::EventManager event_manager;
    sf::Vector2f       mouse_position;

    Item root;

    explicit
    Scene(sf::RenderWindow& window_)
        : window(window_)
        , event_manager(window_, false)
        , root(toVector2f(window_.getSize()))
    {
        const auto size = window_.getSize();
        initializeEventsCallbacks();
    }

    template<typename T, typename TCallback>
    void watch(SPtr<T> item, const TCallback&& callback)
    {
        item->observers.push_back({nullptr, callback});
    }
    
    void initializeEventsCallbacks()
    {
        event_manager.addEventCallback(sf::Event::Closed, [&](const sf::Event&) {window.close();});
        event_manager.addEventCallback(sf::Event::MouseButtonPressed, [&](const sf::Event& e) {dispatchClick(e);});
        event_manager.addEventCallback(sf::Event::MouseButtonReleased, [&](const sf::Event& e) {unclick(e);});
        event_manager.addEventCallback(sf::Event::MouseMoved, [&](const sf::Event& e) {mouseMove(e.mouseMove.x, e.mouseMove.y);});
        event_manager.addEventCallback(sf::Event::KeyPressed, [&](const sf::Event& e) {processKeyPressed(e);});
    }
    
    void processKeyPressed(const sf::Event& e)
    {
        if (e.key.code == sf::Keyboard::Escape) {
            window.close();
        } else {
            root.executeCallback(e);
        }
    }
    
    void processEvents()
    {
        mouse_position = toVector2f(sf::Mouse::getPosition(event_manager.getWindow()));
        event_manager.processEvents([&](const sf::Event& e){
            root.executeCallback(e);
        });
    }
    
    void dispatchClick(const sf::Event& e)
    {
        root.defaultOnClick(mouse_position, e.mouseButton.button);
    }
    
    void unclick(const sf::Event& e)
    {
        root.defaultOnUnclick(e.mouseButton.button);
    }
    
    void update()
    {
        processEvents();
        root.defaultUpdate();
    }
    
    void addItem(GUI::ItemPtr item, const std::string& name = "", Alignement alignement = Alignement::None)
    {
        root.addItem(item, name, alignement);
    }
    
    template<typename T>
    std::shared_ptr<T> getByName(const std::string& name)
    {
        return root.getByName<T>(name);
    }
    
    void render()
    {
        root.defaultRender(window);
    }
    
    void mouseMove(int32_t x, int32_t y)
    {
        mouse_position = sf::Vector2f(to<float>(x), to<float>(y));
        root.defaultOnMouseMove(mouse_position);
    }
};

}
