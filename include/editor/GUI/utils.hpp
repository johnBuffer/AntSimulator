#pragma once
#include <common/utils.hpp>
#include <SFML/Graphics.hpp>


template<typename T>
sf::Vector2f toVector2f(sf::Vector2<T> v)
{
    return sf::Vector2f(to<float>(v.x), to<float>(v.y));
}


template<typename T>
T& getX(sf::Vector2<T>& v)
{
    return v.x;
}


template<typename T>
T& getY(sf::Vector2<T>& v)
{
    return v.y;
}


template<typename T>
using CoordAccessor = std::function<T&(sf::Vector2<T>&)>;


template<typename T>
static sf::Color createColor(T r, T g, T b)
{
    return sf::Color{ to<uint8_t>(r), to<uint8_t>(g), to<uint8_t>(b) };
}
