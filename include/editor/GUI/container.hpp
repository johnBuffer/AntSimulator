#pragma once
#include "GUI/item.hpp"


namespace GUI
{

struct Container : public Item
{
    enum class Orientation
    {
        Horizontal,
        Vertical
    };
    
    Orientation orientation;
    float spacing = 10.0f;
    CoordAccessor<float> coord_1_accessor;
    CoordAccessor<float> coord_2_accessor;
    CoordAccessor<Size> size_type_coord_1_accessor;
    CoordAccessor<Size> size_type_coord_2_accessor;
    
    Container(Orientation orientation_, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : Item(size_, position_)
        , orientation(orientation_)
    {
        coord_1_accessor = (orientation == Orientation::Horizontal) ? getX<float> : getY<float>;
        coord_2_accessor = (orientation == Orientation::Horizontal) ? getY<float> : getX<float>;
        size_type_coord_1_accessor = (orientation == Orientation::Horizontal) ? getX<Size> : getY<Size>;
        size_type_coord_2_accessor = (orientation == Orientation::Horizontal) ? getY<Size> : getX<Size>;
    }
    
    void addItem(ItemPtr item, const std::string& name = "")
    {
        Item::addItem(item, name);
        watchSize(item, [this, item](){
            updateItems();
        });
        updateItems();
    }
    
    void onSizeChange() override
    {
        updateItems();
    }
    
    void onPositionChange() override
    {
        updateItems();
    }
    
    Size& getCoord1(sf::Vector2<Size>& v)
    {
        return size_type_coord_1_accessor(v);
    }
    
    Size& getCoord2(sf::Vector2<Size>& v)
    {
        return size_type_coord_2_accessor(v);
    }
    
    float& getCoord1(sf::Vector2f& v)
    {
        return coord_1_accessor(v);
    }
    
    float& getCoord2(sf::Vector2f& v)
    {
        return coord_2_accessor(v);
    }
    
    sf::Vector2f getItemCoord2Size(ItemPtr item, float coord_2_size)
    {
        sf::Vector2f new_size = item->size;
        if (getCoord2(item->size_type) == Size::Auto) {
            getCoord2(new_size) = coord_2_size;
        }
        return new_size;
    }
    
    void setItemSize(ItemPtr item, float coord_1_size, float coord_2_size)
    {
    }
    
    void updateItems()
    {
        if (sub_items.empty()) {
            return;
        }

        bool        all_fixed = true;
        float       this_coord_2_size = getCoord2(size);
        float       current_pos = padding;
        const float coord_1_size = getAutoItemSize();
        const float coord_2_size = this_coord_2_size - 2.0f * padding;
        // Iterate over items and update sizes
        for (ItemPtr item : sub_items) {
            // Update item
            sf::Vector2f new_item_size = getItemCoord2Size(item, coord_2_size);
            if (getCoord1(item->size_type) == Size::Auto) {
                getCoord1(new_item_size) = coord_1_size;
                all_fixed = false;
            }
            item->setPosition(makeVector2(current_pos, padding));
            item->setSize(new_item_size);
            // Update container state
            this_coord_2_size = std::max(this_coord_2_size, getCoord2(new_item_size) + 2.0f * padding);
            current_pos += getCoord1(item->size) + spacing;
        }
        // If needed, resize container
        sf::Vector2f new_size = size;
        if (getCoord1(size_type) == Size::FitContent && all_fixed) {
            getCoord1(new_size) = current_pos - spacing + padding;
        }
        if (getCoord2(size_type) == Size::FitContent) {
            getCoord2(new_size) = this_coord_2_size;
        }
        setSize(new_size);
    }
        
    float getAutoItemSize()
    {
        const uint64_t items_count = sub_items.size();
        uint64_t items_auto_count = items_count;
        float remaining_size = getCoord1(size) - 2.0f * padding;
        // Check for item size type
        for (ItemPtr item : sub_items) {
            const bool is_fixed = getCoord1(item->size_type) != Size::Auto;
            remaining_size -= getCoord1(item->size) * is_fixed;
            items_auto_count -= is_fixed;
        }
        // Compute auto sized item new size
        remaining_size -= spacing * to<float>(items_count - 1);
        return remaining_size / to<float>(items_auto_count);
    }
    
    template<typename T>
    sf::Vector2<T> makeVector2(T coord_1, T coord_2)
    {
        sf::Vector2<T> res;
        getCoord1(res) = coord_1;
        getCoord2(res) = coord_2;
        return res;
    }
};

}
