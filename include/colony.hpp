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
	Cooldown pop_diff_update;
	RDiff<int64_t> pop_diff;
	uint8_t id;
	sf::Color ants_color = sf::Color::White;


	Colony(float x, float y, uint32_t n, uint8_t col_id)
		: base(sf::Vector2f(x, y), 20.0f)
		, max_ants_count(n)
		, ants_creation_cooldown(0.125f)
		, food_acc(0.0f)
		, food_acc_mean(100)
		, pop_diff_update(1.0f)
		, pop_diff(60)
		, id(col_id)
	{
		base.food = 0.0f;
		uint32_t ants_count = 128;
		for (uint32_t i(ants_count); i--;) {
			createAnt(base.position, getRandRange(2.0f * PI));
		}
	}

	void createAnt(sf::Vector2f pos, float angle)
	{
		ants.emplace_back(pos.x, pos.y, angle, id);
	}

	void update(float dt, World& world)
	{
		pop_diff_update.update(dt);
		if (pop_diff_update.ready()) {
			pop_diff_update.reset();
 			pop_diff.addValue(ants.size());
		}

		const float ant_cost = 2.0f;
		ants_creation_cooldown.update(dt);
		if (ants_creation_cooldown.ready() && ants.size() < max_ants_count && base.useFood(ant_cost)) {
			createAnt(base.position, getRandRange(2.0f * PI));
			ants_creation_cooldown.reset();
		}
		
		for (Ant& ant : ants) {
			ant.update(dt, world);
			ant.checkColony(base);
		}

		// Remove dead ants
		auto it = std::remove_if(ants.begin(), ants.end(), [](const Ant& a) { return a.autonomy > a.max_autonomy; });
		ants.erase(it, ants.end());

		base.updateFoodAcc(dt);
	}
};
