#pragma once
#include <SFML/System.hpp>


struct ColonyBase
{
	sf::Vector2f position;
	float radius;
	float food;

	ColonyBase()
		: radius(0.0f)
		, food(0.0f)
	{}

	ColonyBase(sf::Vector2f position_, float r)
		: position(position_)
		, radius(r)
		, food(0.0f)
	{}

	void addFood(float quantity)
	{
		food += quantity;
	}

	bool useFood(float quantity)
	{
		if (food >= quantity) {
			food -= quantity;
			return true;
		}
		return false;
	}
};
