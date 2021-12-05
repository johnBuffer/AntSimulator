#pragma once
#include "GUI/item.hpp"


namespace GUI
{

struct TextLabel : public Item
{
    std::string label;
    sf::Font font;
    sf::Text text;
    uint32_t char_size;
    
    Alignement alignement;
    
    TextLabel(const std::string& label_, uint32_t char_size_, sf::Vector2f size_ = {}, sf::Vector2f position_ = {})
        : Item(position_, size_)
        , label(label_)
        , char_size(char_size_)
        , alignement(Alignement::Center)
    {
        font.loadFromFile("res/font.ttf");
        text.setFont(font);
        text.setFillColor(sf::Color::Black);
        text.setCharacterSize(char_size);
        setText(label);
        setHeight(padding + text.getGlobalBounds().height);
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
    }
    
    void setAlignement(Alignement new_alignement)
    {
        alignement = new_alignement;
        updateOrigin();
        updateTextPosition();
    }
    
    void updateOrigin()
    {
        const auto text_bounds = text.getGlobalBounds();
        switch (alignement) {
            case Alignement::Center:
                text.setOrigin(text_bounds.width * 0.5f - size.x * 0.5f, text_bounds.height - size.y * 0.5f);
                break;
            case Alignement::Left:
                text.setOrigin({});
                break;
            case Alignement::Right:
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
        GUI::Item::draw(target, text);
    }
};

}
