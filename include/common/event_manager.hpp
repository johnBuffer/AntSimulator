#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>

namespace sfev
{

// Helper using for shorter types
using EventCallback = std::function<void(const sf::Event& event)>;

template<typename T>
using EventCallbackMap = std::unordered_map<T, EventCallback>;

using CstEv = const sf::Event&;


/*
    This class handles subtyped events like keyboard or mouse events
    The unpack function allows to get relevant information from the processed event
*/
template<typename T>
class SubTypeManager
{
public:
    SubTypeManager(std::function<T(const sf::Event&)> unpack) :
        m_unpack(unpack)
    {}

    ~SubTypeManager() = default;

    void processEvent(const sf::Event& event) const
    {
        T sub_value = m_unpack(event);
        auto it(m_callmap.find(sub_value));
        if (it != m_callmap.end()) {
            // Call its associated callback
            (it->second)(event);
        }
    }

    void addCallback(const T& sub_value, EventCallback callback)
    {
        m_callmap[sub_value] = callback;
    }

private:
    EventCallbackMap<T> m_callmap;
    std::function<T(const sf::Event&)> m_unpack;
};


class EventMap
{
public:
    EventMap(bool use_builtin_helpers = true)
        : m_key_pressed_manager([](const sf::Event& event) {return event.key.code; })
        , m_key_released_manager([](const sf::Event& event) {return event.key.code; })
        , m_mouse_pressed_manager([](const sf::Event& event) {return event.mouseButton.button; })
        , m_mouse_released_manager([](const sf::Event& event) {return event.mouseButton.button; })
    {
        if (use_builtin_helpers) {
            // Register key events built in callbacks
            this->addEventCallback(sf::Event::EventType::KeyPressed, [&](const sf::Event& event) {m_key_pressed_manager.processEvent(event); });
            this->addEventCallback(sf::Event::EventType::KeyReleased, [&](const sf::Event& event) {m_key_released_manager.processEvent(event); });
            this->addEventCallback(sf::Event::EventType::MouseButtonPressed, [&](const sf::Event& event) {m_mouse_pressed_manager.processEvent(event); });
            this->addEventCallback(sf::Event::EventType::MouseButtonReleased, [&](const sf::Event& event) {m_mouse_released_manager.processEvent(event); });
        }
    }
    
    // Attaches new callback to an event
    void addEventCallback(sf::Event::EventType type, EventCallback callback)
    {
        m_events_callmap[type] = callback;
    }
    
    // Adds a key pressed callback
    void addKeyPressedCallback(sf::Keyboard::Key key_code, EventCallback callback)
    {
        m_key_pressed_manager.addCallback(key_code, callback);
    }

    // Adds a key released callback
    void addKeyReleasedCallback(sf::Keyboard::Key key_code, EventCallback callback)
    {
        m_key_released_manager.addCallback(key_code, callback);
    }

    // Adds a mouse pressed callback
    void addMousePressedCallback(sf::Mouse::Button button, EventCallback callback)
    {
        m_mouse_pressed_manager.addCallback(button, callback);
    }

    // Adds a mouse released callback
    void addMouseReleasedCallback(sf::Mouse::Button button, EventCallback callback)
    {
        m_mouse_released_manager.addCallback(button, callback);
    }
    
    // Runs the callback associated with an event
    void executeCallback(const sf::Event& e, EventCallback fallback = nullptr) const
    {
        auto it(m_events_callmap.find(e.type));
        if (it != m_events_callmap.end()) {
            // Call its associated callback
            (it->second)(e);
        } else if (fallback) {
            fallback(e);
        }
    }
    
    // Removes a callback
    void removeCallback(sf::Event::EventType type)
    {
        // If event type is registred
        auto it(m_events_callmap.find(type));
        if (it != m_events_callmap.end()) {
            // Remove its associated callback
            m_events_callmap.erase(it);
        }
    }
    
private:
    SubTypeManager<sf::Keyboard::Key> m_key_pressed_manager;
    SubTypeManager<sf::Keyboard::Key> m_key_released_manager;
    SubTypeManager<sf::Mouse::Button> m_mouse_pressed_manager;
    SubTypeManager<sf::Mouse::Button> m_mouse_released_manager;
    EventCallbackMap<sf::Event::EventType> m_events_callmap;
};


/*
    This class handles any type of event and call its associated callbacks if any.
    To process key event in a more convenient way its using a KeyManager
*/
class EventManager
{
public:
    EventManager(sf::Window& window, bool use_builtin_helpers) :
        m_window(window),
        m_event_map(use_builtin_helpers)
    {
    }

    // Calls events' attached callbacks
    void processEvents(EventCallback fallback = nullptr) const
    {
        // Iterate over events
        sf::Event event;
        while (m_window.pollEvent(event)) {
            m_event_map.executeCallback(event, fallback);
        }
    }
    
    // Attaches new callback to an event
    void addEventCallback(sf::Event::EventType type, EventCallback callback)
    {
        m_event_map.addEventCallback(type, callback);
    }

    // Removes a callback
    void removeCallback(sf::Event::EventType type)
    {
        m_event_map.removeCallback(type);
    }

    // Adds a key pressed callback
    void addKeyPressedCallback(sf::Keyboard::Key key, EventCallback callback)
    {
        m_event_map.addKeyPressedCallback(key, callback);
    }

    // Adds a key released callback
    void addKeyReleasedCallback(sf::Keyboard::Key key, EventCallback callback)
    {
        m_event_map.addKeyReleasedCallback(key, callback);
    }

    // Adds a mouse pressed callback
    void addMousePressedCallback(sf::Mouse::Button button, EventCallback callback)
    {
        m_event_map.addMousePressedCallback(button, callback);
    }

    // Adds a mouse released callback
    void addMouseReleasedCallback(sf::Mouse::Button button, EventCallback callback)
    {
        m_event_map.addMouseReleasedCallback(button, callback);
    }
    
    sf::Window& getWindow()
    {
        return m_window;
    }

    sf::Vector2f getFloatMousePosition() const
    {
        const sf::Vector2i mouse_position = sf::Mouse::getPosition(m_window);
        return { static_cast<float>(mouse_position.x), static_cast<float>(mouse_position.y) };
    }

    sf::Vector2i getMousePosition() const
    {
        return sf::Mouse::getPosition(m_window);
    }

private:
    sf::Window& m_window;
    EventMap    m_event_map;
};

} // End namespace
