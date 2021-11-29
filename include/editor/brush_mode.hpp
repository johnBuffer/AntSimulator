#include "object.hpp"
#include <iostream>


namespace edtr
{

struct Tool
{
    Object& object;
    
    Tool(Object& object_)
        : object(object_)
    {}
    
    virtual void onToolMove(sf::Vector2f position) {}
    virtual void onClick() {};
    virtual void onUnclick() {};
};

struct Brush : public Tool
{
    float radius = 0.0f;
    
    Brush(Object& object_)
        : Tool(object_)
    {}
    
    void onToolMove(sf::Vector2f position) override
    {
        object.map([&](IVec2 pos, int32_t, Atom& a){
            const sf::Vector2f atom_pos(pos.x + 0.5f, pos.y + 0.5f);
            a.hover = std::abs(atom_pos.x - position.x) < radius && std::abs(atom_pos.y - position.y) < radius;
            //a.hover = getLength(atom_pos - position) < radius;
        });
    }
};

struct BrushCreate : public Brush
{
    BrushCreate(Object& object_)
        : Brush(object_)
    {}
    
    void onClick() override
    {
        object.map([&](IVec2, int32_t, Atom& a){
            if (a.hover && !a.exists) {
                a.create();
            }
        });
    }
};


struct BrushDelete : public Brush
{
    BrushDelete(Object& object_)
        : Brush(object_)
    {}
    
    void onClick() override
    {
        object.map([&](IVec2, int32_t, Atom& a){
            if (a.hover && a.exists) {
                a.remove();
            }
        });
    }
};


struct BrushColor : public Brush
{
    sf::Color color = sf::Color::White;
    BrushColor(Object& object_)
        : Brush(object_)
    {}
    
    void onClick() override
    {
        object.map([&](IVec2, int32_t, Atom& a){
            if (a.hover && a.exists) {
                a.color = color;
            }
        });
    }
};


}
