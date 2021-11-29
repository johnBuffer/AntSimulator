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
        updatePosition();
    }
    
    void setColor(sf::Color color)
    {
        text.setFillColor(color);
    }
    
    void onSizeChange() override
    {
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
                text.setOrigin(text_bounds.width * 0.5f, text_bounds.height);
                break;
            case Alignement::Left:
                text.setOrigin({});
            default:
                break;
        }
    }
    
    void updateTextPosition()
    {
        switch (alignement) {
            case Alignement::Center:
                text.setPosition(position + size * 0.5f);
                break;
            case Alignement::Left:
                text.setPosition(position);
            default:
                break;
        }
    }
    
    void render(sf::RenderTarget& target) override
    {
        GUI::Item::draw(target, text);
    }
};

}
