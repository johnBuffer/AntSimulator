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
	ColonyBase       base;
	uint32_t         max_ants_count;
	civ::Vector<Ant> ants;
	Cooldown         ants_creation_cooldown;
	Cooldown         pop_diff_update;
	RDiff<int64_t>   pop_diff;
	uint8_t          id;
	sf::Color        ants_color       = sf::Color::White;
	uint64_t         ant_creation_id  = 0;
    bool             color_changed    = false;
    bool             position_changed = false;


    Colony() = default;

	Colony(float x, float y, uint32_t n)
		: base(sf::Vector2f(x, y), 20.0f)
		, max_ants_count(n)
		, ants_creation_cooldown(0.125f)
		, pop_diff_update(1.0f)
		, pop_diff(60)
		, id(0)
	{

	}

    void initialize(uint8_t colony_id)
    {
        id = colony_id;
        base.food = 0.0f;
        uint32_t ants_count = 1000;
        for (uint32_t i(ants_count); i--;) {
            createWorker();
        }
    }

    void setPosition(sf::Vector2f new_position)
    {
        position_changed = true;
        base.position = new_position;
        for (Ant& a : ants) {
            a.position = new_position;
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
		ant.type                  = Ant::Type::Soldier;
		ant.length                *= soldier_scale;
		ant.width                 *= soldier_scale;
		ant.damage                *= soldier_scale * 2.0f;
		ant.max_autonomy          *= soldier_scale;
	}

	void genericAntsUpdate(float dt, World& world)
	{
		for (Ant& ant : ants) {
			AntUpdater::initialUpdate(ant, world, dt);
		}
	}

    [[nodiscard]]
	bool mustCreateSoldier() const
	{
		const uint32_t soldiers_creation_discard = 5;
		return base.enemies_found_count && (ant_creation_id % soldiers_creation_discard == 0);
	}

    [[nodiscard]]
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
 			pop_diff.addValue(to<int64_t>(ants.size()));
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
		std::list<int16_t> to_remove;
		for (Ant& a : ants) {
			if (a.isDead()) {
				to_remove.push_back(a.id);
			}
		}
		for (uint64_t ant_id : to_remove) {
			ants.erase(ant_id);
		}
	}
    
    uint32_t killWeakAnts(World& world)
    {
        uint32_t count = 0;
        for (Ant& a : ants) {
            if (a.isDone()) {
                a.kill(world);
                ++count;
            }
        }
        return count;
    }

    [[nodiscard]]
	uint32_t soldiersCount() const
	{
		return to<uint32_t>(std::count_if(ants.begin(), ants.end(), [](const Ant& a) { return a.type == Ant::Type::Soldier; }));
	}

    void setColor(sf::Color color)
    {
        ants_color = color;
        color_changed = true;
    }

    void stopFightsWith(uint8_t colony_id)
    {
        for (Ant& a : ants) {
            if (a.target) {
                a.fight_mode = FightMode::NoFight;
                if (a.target->col_id == colony_id) {
                    a.target = {};
                }
            }
        }
    }
};
