#pragma once


namespace GUI
{

struct EmptyItem : public Item
{
    EmptyItem()
    {
        size_type = {Size::Auto, Size::Auto};
    }

//    void render(sf::RenderTarget& target) override
//    {
//        auto r = sf::RectangleShape(size);
//        r.setPosition(position);
//        r.setFillColor(sf::Color::Red);
//        GUI::Item::draw(target, r);
//    }
};

}
