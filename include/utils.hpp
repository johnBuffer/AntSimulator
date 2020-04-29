#pragma once
#include <SFML/Graphics.hpp>


constexpr float PI = 3.14159265f;


float getRandRange(float width);


float getLength(const sf::Vector2f& v);


float getAngle(const sf::Vector2f& v);


float dot(const sf::Vector2f& v1, const sf::Vector2f& v2);
