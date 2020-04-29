#include "utils.hpp"


float getRandRange(float width)
{
	constexpr int32_t max_value = 10000;
	constexpr int32_t half = max_value / 2;
	constexpr float div = 1.0f / float(max_value);
	const float val = (rand() % max_value - half) * div * width;
	return val;
}


float getLength(const sf::Vector2f& v)
{
	return sqrt(v.x * v.x + v.y * v.y);
}


float getAngle(const sf::Vector2f & v)
{
	const float a = acos(v.x / getLength(v));
	return v.y > 0.0f ? a : -a;
}

float dot(const sf::Vector2f & v1, const sf::Vector2f & v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}
