#pragma once
#include <SFML/System.hpp>
#include "racc.hpp"


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
    uint32_t enemies_found_count = 0;

	ColonyBase()
		: radius(0.0f)
		, food(0.0f)
		, food_acc(0.0f)
		, food_acc_mean(20)
		, food_acc_update(0.1f)
	{}

	ColonyBase(sf::Vector2f position_, float r)
		: position(position_)
		, radius(r)
		, food(0.0f)
		, food_acc(0.0f)
		, food_acc_mean(200)
		, food_acc_update(0.1f)
	{}

	void addFood(float quantity)
	{
		food += quantity;
		food = std::min(max_food, food);
		food_acc += quantity;
	}

	bool useFood(float quantity)
	{
		if (food >= quantity) {
			food -= quantity;
			return true;
		}
		return false;
	}

	void updateFoodAcc(float dt)
	{
		food_acc_update.update(dt);
		if (food_acc_update.ready()) {
			food_acc_update.reset();
			food_acc_mean.addValue(food_acc);
			food_acc = 0.0f;
		}
	}
};
