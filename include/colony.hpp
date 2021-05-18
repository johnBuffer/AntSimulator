#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "ant.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "colony_base.hpp"
#include "graph.hpp"
#include "racc.hpp"


struct Colony
{
	ColonyBase base;
	uint32_t max_ants_count;
	std::vector<Ant> ants;
	Cooldown ants_creation_cooldown;
	float food_acc;
	RMean<float> food_acc_mean;
	Cooldown food_acc_update;


	Colony(float x, float y, uint32_t n)
		: base(sf::Vector2f(x, y), 20.0f)
		, max_ants_count(n)
		, ants_creation_cooldown(0.125f)
		, food_acc(0.0f)
		, food_acc_mean(100)
		, food_acc_update(0.1f)
	{
		base.food = 0.0f;
		uint32_t ants_count = 8;
		for (uint32_t i(ants_count); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI));
		}
	}

	void update(float dt, World& world)
	{
		const float ant_cost = 2.0f;
		ants_creation_cooldown.update(dt);
		if (ants_creation_cooldown.ready() && ants.size() < max_ants_count && base.useFood(ant_cost)) {
			ants.emplace_back(base.position.x, base.position.y, getRandRange(2.0f * PI));
			ants_creation_cooldown.reset();
		}
		
		for (Ant& ant : ants) {
			ant.update(dt, world);
			ant.checkColony(base);
		}

		// Remove dead ants
		auto it = std::remove_if(ants.begin(), ants.end(), [](const Ant& a) { return a.autonomy > a.max_autonomy; });
		ants.erase(it, ants.end());

		food_acc_update.update(dt);
		if (food_acc_update.ready()) {
			food_acc_update.reset();
			food_acc_mean.addValue(food_acc);
			food_acc = 0.0f;
		}
	}
};
