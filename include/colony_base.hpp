#pragma once
#include <SFML/System.hpp>


struct ColonyBase
{
	sf::Vector2f position;
	float radius;
	float food;
	float max_food = 100.0f;
	// Profiling
	float food_acc;
	RMean<float> food_acc_mean;
	Cooldown food_acc_update;

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
		food = std::min(max_food, food);
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
