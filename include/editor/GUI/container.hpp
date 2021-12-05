#pragma once
#include "GUI/item.hpp"
#include "common/color_utils.hpp"


namespace GUI
{

struct AutoItemSize
{
    bool  valid;
    float value;
};

struct Container : public Item
{
    enum class Orientation
    {
        Horizontal,
        Vertical
    };
    
    Orientation orientation;
    float spacing = 8.0f;
    CoordAccessor<float> coord_1_accessor;
    CoordAccessor<float> coord_2_accessor;
    CoordAccessor<Size> size_type_coord_1_accessor;
    CoordAccessor<Size> size_type_coord_2_accessor;

    sf::Color color;

    Container(Orientation orientation_, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : Item(size_, position_)
        , orientation(orientation_)
    {
        coord_1_accessor = (orientation == Orientation::Horizontal) ? getX<float> : getY<float>;
        coord_2_accessor = (orientation == Orientation::Horizontal) ? getY<float> : getX<float>;
        size_type_coord_1_accessor = (orientation == Orientation::Horizontal) ? getX<Size> : getY<Size>;
        size_type_coord_2_accessor = (orientation == Orientation::Horizontal) ? getY<Size> : getX<Size>;
        padding = 8.0f;
    }
    
    void addItem(ItemPtr item, const std::string& name = "")
    {
        Item::addItem(item, name);
        watchSize(item, [this, item](){
            updateItems();
        });
        updateItems();
    }

    void removeItem(SPtr<Item> item)
    {
        for (auto sub = sub_items.begin(); sub != sub_items.end(); ++sub) {
            if (sub->get() == item.get()) {
                sub_items.erase(sub);
                updateItems();
                return;
            }
        }
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

    [[nodiscard]]
    bool isEmpty() const
    {
        return sub_items.empty();
    }
    
    void updateItems()
    {
        bool        all_fixed = true;
        float       this_coord_2_size = getCoord2(size);
        float       current_pos = padding;

        const AutoItemSize auto_size = getAutoItemSize();

        const float coord_2_size = this_coord_2_size - 2.0f * padding;
        // Iterate over items and update sizes
        for (ItemPtr item : sub_items) {
            // Update item
            sf::Vector2f new_item_size = getItemCoord2Size(item, coord_2_size);
            if (getCoord1(item->size_type) == Size::Auto) {
                getCoord1(new_item_size) = auto_size.value;
                all_fixed = false;
            }
            item->setPosition(makeVector2(current_pos, padding + (coord_2_size - getCoord2(item->size)) * 0.5f));
            item->setSize(new_item_size);
            // Update container state
            this_coord_2_size = std::max(this_coord_2_size, getCoord2(new_item_size) + 2.0f * padding);
            current_pos += getCoord1(item->size) + spacing;
        }
        // If needed, resize container
        sf::Vector2f new_size = size;
        if (getCoord1(size_type) == Size::FitContent && all_fixed) {
            getCoord1(new_size) = current_pos + padding - spacing * to<float>(!sub_items.empty());
        }
        if (getCoord2(size_type) == Size::FitContent) {
            getCoord2(new_size) = this_coord_2_size;
        }
        setSize(new_size);
        forceActiveItemUpdate();
    }

    AutoItemSize getAutoItemSize()
    {
        const uint64_t items_count = sub_items.size();
        if (items_count == 0) {
            return {false, 0.0f};
        }
        uint64_t items_auto_count = items_count;
        float remaining_size = getCoord1(size) - 2.0f * padding * to<float>(items_count > 0);
        // Check for item size type
        for (ItemPtr item : sub_items) {
            const bool is_fixed = getCoord1(item->size_type) != Size::Auto;
            remaining_size -= getCoord1(item->size) * to<float>(is_fixed);
            items_auto_count -= is_fixed;
        }
        if (items_auto_count == 0) {
            return AutoItemSize{false, 0.0f};
        }
        // Compute auto sized item new size
        remaining_size -= spacing * to<float>(items_count - 1);
        return {true, remaining_size / to<float>(items_auto_count)};
    }
    
    template<typename T>
    sf::Vector2<T> makeVector2(T coord_1, T coord_2)
    {
        sf::Vector2<T> res;
        getCoord1(res) = coord_1;
        getCoord2(res) = coord_2;
        return res;
    }

//    void render(sf::RenderTarget& target) override
//    {
//        GUI::RoundedRectangle back(size, position, 3.0f);
//        back.setFillColor(color);
//        GUI::Item::draw(target, back);
//    }
};

}
