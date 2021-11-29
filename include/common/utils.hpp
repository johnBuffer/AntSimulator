#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <sstream>


constexpr float PI = 3.14159265f;


float getRandRange(float width);


float getRandUnder(float width);


template<typename T>
float getLength2(sf::Vector2<T> v)
{
	return v.x * v.x + v.y * v.y;
}


template<typename T>
float getLength(sf::Vector2<T> v)
{
	return sqrt(getLength2(v));
}

template<typename T>
sf::Vector2<T> getNormalized(sf::Vector2<T> v)
{
	return v / getLength(v);
}

template<typename T, typename U>
T to(const U& u)
{
	return static_cast<T>(u);
}

template<typename T>
std::string toStr(T v)
{
	std::stringstream sstr;
	sstr << v;
	return sstr.str();
}

template<typename T>
T clamp(T value, T min, T max)
{
	return std::max(min, std::min(value, max));
}

template<typename T>
sf::Color vec3ToColor(const sf::Vector3<T>& v)
{
	return sf::Color(to<uint8_t>(clamp(v.x, 0.0f, 255.0f)),
					 to<uint8_t>(clamp(v.y, 0.0f, 255.0f)),
					 to<uint8_t>(clamp(v.z, 0.0f, 255.0f)));
}

template<typename Vec2Type>
float getAngle(const Vec2Type& v)
{
    const float a = acos(v.x / getLength(v));
    return v.y > 0.0f ? a : -a;
}

template<typename Vec2Type>
float dot(const Vec2Type& v1, const Vec2Type& v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
float sign(T f)
{
    return f < 0.0f ? -1.0f : 1.0f;
}

template<typename T>
using SPtr = std::shared_ptr<T>;


template<typename T, typename ...Args>
SPtr<T> create(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
