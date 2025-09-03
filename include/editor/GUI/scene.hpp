#pragma once
#include "editor/GUI/item.hpp"
#include "utils.hpp"
#include <common/events.hpp>
#include <vector>


namespace GUI
{

struct Scene
{
    using Ptr = std::shared_ptr<Scene>;
    
    sf::RenderWindow& window;
    pez::EventHandler event_manager;
    sf::Vector2f      mouse_position;

    Item root;

    explicit
    Scene(sf::RenderWindow& window_)
        : window(window_)
        , event_manager(window_)
        , root(toVector2f(window_.getSize()) / Conf::GUI_SCALE)
    {
        initializeEventsCallbacks();
    }

    virtual ~Scene() = default;

    template<typename T, typename TCallback>
    void watch(SPtr<T> item, const TCallback&& callback)
    {
        item->observers.push_back({nullptr, callback});
    }
    
    void initializeEventsCallbacks()
    {
        event_manager.addCallback<sf::Event::Closed>([&](const sf::Event&) {window.close();});
        event_manager.addCallback<sf::Event::MouseButtonPressed>([&](const sf::Event::MouseButtonPressed& e) {dispatchClick(e);});
        event_manager.addCallback<sf::Event::MouseButtonReleased>([&](const sf::Event::MouseButtonReleased& e) {unclick(e);});
        event_manager.addCallback<sf::Event::MouseMoved>([&](const sf::Event::MouseMoved& e) { mouseMove(e.position.x, e.position.y); });
        event_manager.addCallback<sf::Event::KeyPressed>([&](const sf::Event::KeyPressed& e) { processKeyPressed(e); });
        event_manager.addCallback<sf::Event::Resized>([this](sf::Event::Resized const&){ resize(); });
    }

    virtual void onSizeChange() {}

    void resize()
    {
        const auto size = window.getSize();
        const sf::Vector2f new_size{to<float>(size.x), to<float>(size.y)};
        root.setSize(new_size / Conf::GUI_SCALE);
        window.setView(sf::View(new_size * 0.5f, new_size));
        onSizeChange();
    }
    
    void processKeyPressed(const sf::Event::KeyPressed& e)
    {
        if (e.code == sf::Keyboard::Key::Escape) {
            window.close();
        } else {
            root.executeCallback(e);
        }
    }
    
    void processEvents()
    {
        mouse_position = toVector2f(sf::Mouse::getPosition(event_manager.getWindow()));
        event_manager.processEvents([&](const sf::Event& e) {
            root.executeCallback(e);
        });
    }
    
    void dispatchClick(const sf::Event::MouseButtonPressed& e)
    {
        root.defaultOnClick(mouse_position / Conf::GUI_SCALE, e.button);
    }
    
    void unclick(const sf::Event::MouseButtonReleased& e)
    {
        root.defaultOnUnclick(e.button);
    }
    
    void update()
    {
        processEvents();
        root.defaultUpdate();
    }
    
    void addItem(GUI::ItemPtr item, const std::string& name = "", Alignment alignement = Alignment::None)
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
        mouse_position = sf::Vector2f(to<float>(x), to<float>(y)) / Conf::GUI_SCALE;
        root.defaultOnMouseMove(mouse_position);
    }
};

}
