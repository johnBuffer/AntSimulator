#include "utils.hpp"
#include <random>

std::random_device rd();
std::mt19937 gen(0);

float getRandRange(float width)
{
	std::uniform_real_distribution<float> distr(-width, width);
	return distr(gen);
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
