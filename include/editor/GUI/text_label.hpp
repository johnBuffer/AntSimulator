#pragma once
#include "editor/GUI/item.hpp"


namespace GUI
{

struct TextLabel : public Item
{
    std::string label;
    sf::Font font;
    sf::Text text;
    uint32_t char_size;
    bool auto_size_update = true;
    
    Alignment alignment;
    
    TextLabel(const std::string& label_, uint32_t char_size_, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : Item(position_, size_)
        , label(label_)
        , char_size(char_size_)
        , alignment(Alignment::Center)
    {
        padding = 1.0f;
        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setCharacterSize(char_size);
        setColor({100, 100, 100});
        setText(label);
        updatePosition();
    }
    
    void setColor(sf::Color color)
    {
        text.setFillColor(color);
    }
    
    void onSizeChange() override
    {
        updateOrigin();
        updateTextPosition();
    }
    
    void onPositionChange() override
    {
        updateTextPosition();
    }
    
    void setText(const std::string& str)
    {
        text.setString(str);
        updateOrigin();
        if (auto_size_update) {
            updateSize();
        }
    }
    
    void setAlignment(Alignment new_alignment)
    {
        alignment = new_alignment;
        updateOrigin();
        updateTextPosition();
    }

    void updateSize()
    {
        setHeight(2.0f * padding + text.getGlobalBounds().height);
        setWidth(2.0f * padding + text.getGlobalBounds().width);
    }
    
    void updateOrigin()
    {
        const auto text_bounds = text.getGlobalBounds();
        switch (alignment) {
            case Alignment::Center:
                text.setOrigin(text_bounds.width * 0.5f - size.x * 0.5f, text_bounds.height - size.y * 0.5f);
                break;
            case Alignment::Left:
                text.setOrigin({});
                break;
            case Alignment::Right:
                text.setOrigin(text_bounds.width - size.x, 0.0f);
                break;
            default:
                break;
        }
    }
    
    void updateTextPosition()
    {
        text.setPosition(position);
    }
    
    void render(sf::RenderTarget& target) override
    {
//        auto r = sf::RectangleShape(size);
//        r.setPosition(position);
//        GUI::Item::draw(target, r);
        GUI::Item::draw(target, text);
    }
};

}
