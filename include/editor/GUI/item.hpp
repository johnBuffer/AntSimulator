#pragma once
#include <SFML/Graphics.hpp>
#include <common/event_manager.hpp>
#include "utils.hpp"


namespace GUI
{

enum class Alignement
{
    None,
    Left,
    Right,
    Center
};

enum class Size
{
    Auto,
    FitContent,
    Fixed
};

struct Item;
using ItemPtr = std::shared_ptr<Item>;

struct Observer
{
    Item* observer = nullptr;
    std::function<void(void)> callback = nullptr;
};


struct Item
{
    // Attributes
    sf::Vector2f size;
    sf::Vector2f offset;
    sf::Vector2f position;
    std::string  name;
    float padding = 20.0f;
    bool clicking = false;
    bool catch_event = true;
    sfev::EventMap event_callbacks;
    sf::Vector2<Size> size_type = sf::Vector2<Size>(Size::Auto, Size::Auto);
    // Sub Items
    std::vector<ItemPtr> sub_items;
    std::vector<Observer> observers;
    std::vector<Observer> size_observers;
    ItemPtr active_item = nullptr;
    sf::Vector2f current_mouse_position;

    Item() = default;

    explicit
    Item(sf::Vector2f size_, sf::Vector2f position_ = {}, const std::string& name_ = "")
        : size(size_)
        , position(position_)
        , name(name_)
    {}
    
    virtual void onClick(sf::Vector2f click_position, sf::Mouse::Button button) {}
    virtual void onUnclick(sf::Mouse::Button button) {}
    virtual void onMouseMove(sf::Vector2f new_mouse_position) {}
    virtual void onSizeChange() {}
    virtual void onPositionChange() {}
    virtual void onMouseIn() {}
    virtual void onMouseOut() {}
    virtual void render(sf::RenderTarget& target) {}
    virtual void initializeEventCallbacks() {}
    virtual void update() {}
    
    void defaultOnSizeChange(bool ignore_size_changed = false)
    {
        onSizeChange();
        if (!ignore_size_changed) {
            for (Observer& obs : size_observers) {
                obs.callback();
            }
        }
    }
    
    void setSize(sf::Vector2f new_size, bool ignore_size_changed = false)
    {
        if (size != new_size) {
            size = new_size;
            defaultOnSizeChange(ignore_size_changed);
        }
    }
    
    void setWidth(float w, Size width_type = Size::Fixed)
    {
        const bool has_changed = size.x != w || size_type.x != width_type;
        if (has_changed) {
            size.x = w;
            size_type.x = width_type;
            defaultOnSizeChange();
        }
    }
    
    void setHeight(float h, Size height_type = Size::Fixed)
    {
        const bool has_changed = size.y != h || size_type.y != height_type;
        if (has_changed) {
            size.y = h;
            size_type.y = height_type;
            defaultOnSizeChange();
        }
    }
    
    void setPosition(sf::Vector2f new_position)
    {
        position = new_position;
        updatePosition();
    }
    
    void updatePosition()
    {
        onPositionChange();
        for (ItemPtr item : sub_items) {
            item->setOffset(offset + position);
        }
    }
    
    void updateActiveItem()
    {
        if (active_item && active_item->clicking) {
            return;
        }
        for (auto it = sub_items.rbegin(); it != sub_items.rend(); ++it) {
            ItemPtr item = *it;
            if (item->isUnder(current_mouse_position) && item->catch_event) {
                if (item != active_item) {
                    exitActiveItem();
                    active_item = item;
                    active_item->onMouseIn();
                }
                return;
            }
        }
        exitActiveItem();
    }
    
    void defaultUpdate()
    {
        for (ItemPtr item : sub_items) {
            item->defaultUpdate();
        }
        update();
    }
    
    void defaultOnClick(sf::Vector2f mouse_position, sf::Mouse::Button button)
    {
        clicking = true;
        // Check for click falls in a sub item
        if (active_item) {
            active_item->defaultOnClick(getRelativeMousePosition(active_item, mouse_position),
                                        button);
            return;
        }
        // If click hasn't been intercepted execute callback
        onClick(mouse_position, button);
    }
    
    void defaultOnUnclick(sf::Mouse::Button button)
    {
        // Check for click falls in a sub item
        if (active_item) {
            active_item->defaultOnUnclick(button);
        }
        // If click hasn't been intercepted execute callback
        if (clicking) {
            clicking = false;
            onUnclick(button);
        }
    }
    
    void defaultOnMouseMove(sf::Vector2f mouse_position)
    {
        current_mouse_position = mouse_position;
        updateActiveItem();
        if (active_item) {
            active_item->defaultOnMouseMove(getRelativeMousePosition(active_item, mouse_position));
            return;
        }
        onMouseMove(mouse_position);
    }

    void forceActiveItemUpdate()
    {
        defaultOnMouseMove(current_mouse_position);
    }
    
    void defaultRender(sf::RenderTarget& target)
    {
        render(target);
        for (ItemPtr item : sub_items) {
            item->defaultRender(target);
        }
    }
    
    void draw(sf::RenderTarget& target, const sf::Drawable& drawable)
    {
        sf::RenderStates states;
        draw(target, drawable, states);
    }
    
    void draw(sf::RenderTarget& target, const sf::Drawable& drawable, const sf::RenderStates& states)
    {
        sf::RenderStates default_states = states;
        default_states.transform.translate(offset);
        target.draw(drawable, default_states);
    }
        
    bool isUnder(sf::Vector2f pos) const
    {
        return sf::FloatRect(position, size).contains(pos);
    }
    
    void addItem(ItemPtr item, const std::string& name = "", Alignement alignement = Alignement::None)
    {
        sub_items.push_back(item);
        item->offset = offset + position;
        item->initializeEventCallbacks();
        item->name = name;
        switch (alignement) {
            case Alignement::Right:
                item->setPosition(sf::Vector2f(size.x - item->size.x, 0.0f) + sf::Vector2f(-padding, padding));
                break;
            default:
                break;
        }
    }

    virtual void removeItem(const std::string& item_name)
    {
        for (auto it = sub_items.begin(); it != sub_items.end(); ++it) {
            if ((*it)->name == item_name) {
                sub_items.erase(it);
                return;
            }
        }
    }
    
    template<typename T>
    std::shared_ptr<T> getByName(const std::string& name)
    {
        for (ItemPtr item : sub_items) {
            if (item->name == name) {
                return std::dynamic_pointer_cast<T>(item);
            }
        }
        return nullptr;
    }
    
    void watch(ItemPtr item, std::function<void(void)> callback)
    {
        item->observers.push_back({this, callback});
    }
    
    void watchSize(ItemPtr item, std::function<void(void)> callback)
    {
        item->size_observers.push_back({this, callback});
    }
    
    void notifyChanged()
    {
        for (Observer& obs : observers) {
            obs.callback();
        }
    }
    
    // Attaches new callback to an event
    void addEventCallback(sf::Event::EventType type, sfev::EventCallback callback)
    {
        event_callbacks.addEventCallback(type, callback);
    }

    // Removes a callback
    void removeCallback(sf::Event::EventType type)
    {
        event_callbacks.removeCallback(type);
    }

    // Adds a key pressed callback
    void addKeyPressedCallback(sf::Keyboard::Key key, sfev::EventCallback callback)
    {
        event_callbacks.addKeyPressedCallback(key, callback);
    }

    // Adds a key released callback
    void addKeyReleasedCallback(sf::Keyboard::Key key, sfev::EventCallback callback)
    {
        event_callbacks.addKeyReleasedCallback(key, callback);
    }

    // Adds a mouse pressed callback
    void addMousePressedCallback(sf::Mouse::Button button, sfev::EventCallback callback)
    {
        event_callbacks.addMousePressedCallback(button, callback);
    }

    // Adds a mouse released callback
    void addMouseReleasedCallback(sf::Mouse::Button button, sfev::EventCallback callback)
    {
        event_callbacks.addMouseReleasedCallback(button, callback);
    }
    
    // Runs the callback associated with an event
    void executeCallback(const sf::Event& e) const
    {
        if (active_item) {
            active_item->executeCallback(e);
        }
        else {
            event_callbacks.executeCallback(e);
        }
    }
    
    sf::Vector2f getRelativeMousePosition(ItemPtr item, sf::Vector2f mouse_position) const
    {
        const sf::Vector2f item_size = item->size;
        const sf::Vector2f rel_mouse_position = mouse_position - item->position;
        return sf::Vector2f(clamp(rel_mouse_position.x, 0.0f, item_size.x), clamp(rel_mouse_position.y, 0.0f, item_size.y));
    }
    
    void setOffset(sf::Vector2f new_offset)
    {
        offset = new_offset;
        for (ItemPtr item : sub_items) {
            item->setOffset(offset + position);
        }
    }
    
    void ignoreEvents(bool apply_to_sub_items = true)
    {
        catch_event = false;
        if (apply_to_sub_items) {
            for (ItemPtr item : sub_items) {
                item->ignoreEvents();
            }
        }
    }
    
    void exitActiveItem()
    {
        if (active_item) {
            active_item->exitActiveItem();
            active_item->defaultOnUnclick(sf::Mouse::Button::Left);
            active_item->onMouseOut();
            active_item = nullptr;
        }
    }
};

}
