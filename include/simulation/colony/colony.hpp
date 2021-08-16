#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "common/utils.hpp"
#include "colony_base.hpp"
#include "common/graph.hpp"
#include "common/racc.hpp"
#include "common/index_vector.hpp"
#include "simulation/ant/ant_updater.hpp"


struct Colony
{
	ColonyBase base;
	uint32_t max_ants_count;
	civ::Vector<Ant> ants;
	Cooldown ants_creation_cooldown;
	float food_acc;
	RMean<float> food_acc_mean;
	Cooldown pop_diff_update;
	RDiff<int64_t> pop_diff;
	uint8_t id;
	sf::Color ants_color = sf::Color::White;
	uint64_t ant_creation_id = 0;
    

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
		uint32_t ants_count = 16;
		for (uint32_t i(ants_count); i--;) {
			createWorker();
		}
	}

	Ant& createWorker()
	{
		++ant_creation_id;
		const uint64_t ant_id = ants.emplace_back(base.position.x, base.position.y, RNGf::getUnder(2.0f * PI), id);
		Ant& ant = ants[ant_id];
		ant.id = to<uint16_t>(ant_id);
		ant.type = Ant::Type::Worker;
		return ant;
	}

	void specializeSoldier(Ant& ant)
	{
		--base.enemies_found_count;
		const float soldier_scale = 2.0f;
		ant.type = Ant::Type::Soldier;
		ant.length *= soldier_scale;
		ant.width *= soldier_scale;
		ant.dammage *= soldier_scale * 2.0f;
		ant.max_autonomy *= soldier_scale;
	}

	void genericAntsUpdate(float dt, World& world)
	{
		for (Ant& ant : ants) {
			AntUpdater::initialUpdate(ant, world, dt);
		}
	}

	bool mustCreateSoldier() const
	{
		const uint32_t soldiers_creation_discard = 5;
		return base.enemies_found_count && (ant_creation_id % soldiers_creation_discard == 0);
	}

	bool isNotFull() const
	{
		return ants.size() < max_ants_count;
	}

	void createNewAnts(float dt)
	{
		const float ant_cost = 4.0f;
		if (ants_creation_cooldown.updateAutoReset(dt) && isNotFull()) {
			if (mustCreateSoldier()) {
				if (base.useFood(3.0f * ant_cost)) {
					specializeSoldier(createWorker());
				}
			}
			else if (base.useFood(ant_cost)) {
				createWorker();
			}
		}
	}

	void update(float dt, World& world)
	{
		// Update stats
		if (pop_diff_update.updateAutoReset(dt)) {
 			pop_diff.addValue(ants.size());
		}
		createNewAnts(dt);
		// Update ants and check if collision with colony
		for (Ant& ant : ants) {
			AntUpdater::update(ant, world, dt);
			ant.checkColony(base);
		}
	}

	void removeDeadAnts()
	{
		
	}
    
    void killWeakAnts(World& world)
    {
        // Mark weak ants as dead
        uint32_t dead_count = 0;
        for (Ant& a : ants) {
            if (a.isDone()) {
                a.kill(world);
                ++dead_count;
            }
        }
        // Clean the memory
        std::vector<int16_t> to_remove;
        to_remove.reserve(dead_count);
        for (Ant& a : ants) {
            if (a.isDead()) {
                to_remove.push_back(a.id);
            }
        }
        for (uint64_t ant_id : to_remove) {
            ants.erase(ant_id);
        }
    }
};
