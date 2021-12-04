#pragma once
#include "editor/GUI/container.hpp"
#include "common/color_utils.hpp"

namespace edtr
{

struct ColorVariation : public GUI::Item
{
    sf::Color color;
    sf::Color current_color;
    sf::Color selected_color;
    sf::VertexArray color_va;
    sf::Vector2f selection;

    explicit
    ColorVariation(sf::Vector2f size_ = {})
        : GUI::Item(size_)
        , color_va(sf::Quads, 8)
        , color(sf::Color::Red)
        , selection(0.0f, 0.0f)
    {
        initializeVA();
        updateSelectedColor();
    }
    
    void initializeVA()
    {
        color_va[0].position = position;
        color_va[1].position = position + sf::Vector2f(size.x, 0.0f);
        color_va[2].position = position + size;
        color_va[3].position = position + sf::Vector2f(0.0f, size.y);
        color_va[4].position = position;
        color_va[5].position = position + sf::Vector2f(size.x, 0.0f);
        color_va[6].position = position + size;
        color_va[7].position = position + sf::Vector2f(0.0f, size.y);
        
        color_va[0].color = sf::Color::White;
        color_va[1].color = color;
        color_va[2].color = color;
        color_va[3].color = sf::Color::White;
        color_va[4].color = sf::Color(0, 0, 0, 0);
        color_va[5].color = sf::Color(0, 0, 0, 0);
        color_va[6].color = sf::Color(0, 0, 0, 255);
        color_va[7].color = sf::Color(0, 0, 0, 255);
    }
    
    void onSizeChange() override
    {
        updateSelectedColor();
        initializeVA();
    }
    
    void onPositionChange() override
    {
        initializeVA();
    }

    [[nodiscard]]
    sf::Color getColorAt(sf::Vector2f mouse_position) const
    {
        const float ratio_x = 1.0f - mouse_position.x / size.x;
        const float ratio_y = 1.0f - mouse_position.y / size.y;

        return ColorUtils::createColor((to<float>(color.r) + to<float>(255 - color.r) * ratio_x) * ratio_y,
                                       (to<float>(color.g) + to<float>(255 - color.g) * ratio_x) * ratio_y,
                                       (to<float>(color.b) + to<float>(255 - color.b) * ratio_x) * ratio_y);
    }
    
    void updateSelectedColor()
    {
        selected_color = getColorAt(selection);
        current_color = selected_color;
        notifyChanged();
    }
    
    void onClick(sf::Vector2f mouse_position, sf::Mouse::Button) override
    {
        selection = mouse_position;
        updateSelectedColor();
    }
    
    void setColor(sf::Color new_color)
    {
        color = new_color;
        color_va[1].color = color;
        color_va[2].color = color;
        updateSelectedColor();
    }
    
    void render(sf::RenderTarget& target) override
    {
        draw(target, color_va);
        const float selection_radius = 8.0f;
        sf::CircleShape c(selection_radius);
        c.setOrigin(selection_radius, selection_radius);
        c.setPosition(position + selection);
        c.setFillColor(current_color);
        c.setOutlineColor(sf::Color::White);
        c.setOutlineThickness(2.0f);
        draw(target, c);
    }
    
    void onMouseMove(sf::Vector2f mouse_position) override
    {
        if (clicking) {
            selection = mouse_position;
            updateSelectedColor();
        }
    }
};


struct HueSlider : public GUI::Item
{
    std::vector<sf::Color> colors;
    sf::VertexArray hues_va;
    sf::Color selected_color = sf::Color::Red;

    explicit
    HueSlider(sf::Vector2f size_ = {})
        : GUI::Item(size_)
        , hues_va(sf::TriangleStrip, 14)
    {
        colors = {sf::Color::Red, sf::Color::Yellow, sf::Color::Green, sf::Color::Cyan, sf::Color::Blue, sf::Color::Magenta, sf::Color::Red};
        updateVA();
    }
    
    void updateVA()
    {
        const float hue_length = size.x / to<float>(colors.size() - 1);
        int32_t i = 0;
        for (const sf::Color& c : colors) {
            hues_va[2 * i + 0].position = position + sf::Vector2f(to<float>(i) * hue_length, 0.0f);
            hues_va[2 * i + 1].position = position + sf::Vector2f(to<float>(i) * hue_length, size.y);
            hues_va[2 * i + 0].color = c;
            hues_va[2 * i + 1].color = c;
            ++i;
        }
    }
    
    void onSizeChange() override
    {
        updateVA();
    }
    
    void updateSelectedColor(sf::Vector2f mouse_position)
    {
        const float hue_length = size.x / to<float>(colors.size() - 1);
        const float color_div = mouse_position.x / hue_length;
        const auto color_index = to<uint32_t>(color_div);
        const sf::Color color_1 = colors[color_index];
        const sf::Color color_2 = colors[color_index + 1];
        const float ratio = color_div - to<float>(color_index);
        selected_color = createColor((1.0f - ratio) * to<float>(color_1.r) + ratio * to<float>(color_2.r),
                                     (1.0f - ratio) * to<float>(color_1.g) + ratio * to<float>(color_2.g),
                                     (1.0f - ratio) * to<float>(color_1.b) + ratio * to<float>(color_2.b));
        notifyChanged();
    }
    
    void onClick(sf::Vector2f mouse_position, sf::Mouse::Button) override
    {
        updateSelectedColor(mouse_position);
    }
    
    void onMouseMove(sf::Vector2f mouse_position) override
    {
        if (clicking) {
            updateSelectedColor(mouse_position);
        }
    }
    
    void onPositionChange() override
    {
        updateVA();
    }
    
    void render(sf::RenderTarget& target) override
    {
        draw(target, hues_va);
    }
};


struct ColorPicker : public GUI::Container
{
    SPtr<ColorVariation> color_variation;
    SPtr<HueSlider> hue_slider;

    explicit
    ColorPicker(sf::Vector2f size_ = {})
        : GUI::Container(GUI::Container::Orientation::Vertical, size_)
    {
        padding = 0.0f;
        spacing = 0.0f;
        size_type.y = GUI::Size::FitContent;
        // Create sub elemets
        const float hue_slider_height = 20.0f;
        color_variation = create<ColorVariation>();
        hue_slider = create<HueSlider>();
        hue_slider->setHeight(hue_slider_height);
        
        addItem(color_variation, "Variation");
        addItem(hue_slider, "Hue slider");

        // Add callbacks to sub_items changes
        watch(hue_slider, [this]() {
            color_variation->setColor(hue_slider->selected_color);
        });
        
        watch(color_variation, [this]() {
            notifyChanged();
        });
    }
    
    void onSizeChange() override
    {
        //setHeight(size.x);
    }
    
    sf::Color getColor() const
    {
        return color_variation->selected_color;
    }
    
    void setColor(sf::Color new_color)
    {
        
    }
};

}
