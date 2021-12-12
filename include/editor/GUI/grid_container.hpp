#pragma once
#include "editor/GUI/item.hpp"


namespace GUI
{

struct GridContainer : public Item
{
    sf::Vector2f cursor;
    sf::Vector2f spacing;
    sf::Vector2i grid_size;
    sf::Vector2f items_size;
    
    GridContainer(sf::Vector2f size_, sf::Vector2i grid_size_, sf::Vector2f position_ = {})
        : Item(size_, position_)
        , grid_size(grid_size_)
    {
        spacing = sf::Vector2f(10.0f, 10.0f);
        cursor = sf::Vector2f(padding, padding);
        updateItemsSize();
    }
    
    void updateItemsSize()
    {
        const sf::Vector2f available_size = size - sf::Vector2f(to<float>(grid_size.x - 1) * spacing.x,
                                                                to<float>(grid_size.y - 1) * spacing.y);
        items_size.x = available_size.x / to<float>(grid_size.x);
        items_size.y = available_size.y / to<float>(grid_size.y);
    }
    
    void onSizeChange() override
    {
        updateItems();
    }
    
    void updateItems()
    {
        updateItemsSize();
        uint64_t i(0);
        for (ItemPtr item : sub_items) {
            updateItem(item, i++);
        }
    }
    
    void addItem(ItemPtr item, const std::string& item_name = "")
    {
        // Compute item grid coords
        const uint64_t items_count = sub_items.size();
        Item::addItem(item, item_name, Alignment::None);
        updateItem(item, items_count);
    }
    
    void updateItem(ItemPtr item, uint64_t i)
    {
        const uint64_t item_grid_x = i % grid_size.x;
        const uint64_t item_grid_y = i / grid_size.x;
        // Compute item position
        const sf::Vector2f item_position(item_grid_x * (items_size.x + spacing.x),
                                         item_grid_y * (items_size.y + spacing.y));
        // Setup the item
        item->setPosition(item_position);
        item->setSize(items_size);
    }
    
    uint32_t getMaxItemsCount() const
    {
        return grid_size.x * grid_size.y;
    }
};

}
