#include "utils.hpp"
#include <random>

std::random_device rd();
std::mt19937 gen(0);

float getRandRange(float width)
{
	std::uniform_real_distribution<float> distr(-width, width);
	return distr(gen);
}

float getRandUnder(float width)
{
	std::uniform_real_distribution<float> distr(0.0f, width);
	return distr(gen);
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

float sign(const float f)
{
	return f < 0.0f ? -1.0f : 1.0f;
}
