#pragma once
#include "ant.hpp"
#include "simulation/world/world.hpp"
#include "soldier.hpp"
#include "worker.hpp"


struct AntUpdater
{
	static void initialUpdate(Ant& ant, World& world, float dt)
	{
		// Generic updates
		ant.updateClocks(dt);
		// Update current direction
		ant.direction.update(dt);
		// Add ant to current cell
		ant.addToWorldGrid(world);
	}

	static void update(Ant& ant, World& world, float dt)
	{
		// Fight if needed
		if (ant.isFighting()) {
			ant.attack(dt);
			return;
		}
		ant.updatePosition(world, dt);
		// If fight found, go for it
		if (ant.fight_mode == FightMode::ToFight) {
			ant.to_fight_time += dt;
			if (ant.to_fight_time > ant.to_fight_timeout) {
				ant.fight_mode = FightMode::NoFight;
			}
			return;
		}
		// Refill if too weak
		if (ant.autonomy > 0.75f * ant.max_autonomy) {
			ant.phase = Mode::Refill;
		}
		// Specific updates
		if (ant.type == Ant::Type::Worker) {
			WorkerUpdater::update(ant, world, dt);
		}
		else {
			SoldierUpdater::update(ant, world, dt);
		}
	}
};
