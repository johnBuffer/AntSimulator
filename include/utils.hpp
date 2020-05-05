#pragma once
#include <SFML/Graphics.hpp>


constexpr float PI = 3.14159265f;


float getRandRange(float width);


float getRandUnder(float width);


template<typename T>
float getLength(const sf::Vector2<T>& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}


template<typename T, typename U>
T to(const U& u)
{
	return static_cast<T>(u);
}


float getAngle(const sf::Vector2f& v);


float dot(const sf::Vector2f& v1, const sf::Vector2f& v2);


float sign(const float f);
