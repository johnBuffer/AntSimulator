#pragma once
#include "ant.hpp"
#include "simulation/context.hpp"
#include "soldier.hpp"
#include "worker.hpp"


struct AntUpdater
{
	static void initialUpdate(Ant& ant, World& world, sim::Context& context)
	{
		// Generic updates
		ant.updateClocks(context.dt);
		// Update current direction
		ant.direction.update(context.dt);
		// Add ant to current cell
		ant.addToWorldGrid(world);
	}

	static void update(Ant& ant, World& world, sim::Context& context)
	{
		// Fight if needed
		if (ant.isFighting()) {
			ant.attack(context.dt);
			return;
		}
		ant.updatePosition(world, context.dt);
		// If fight found, go for it
		if (ant.fight_mode == FightMode::ToFight) {
			ant.to_fight_time += context.dt;
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
			WorkerUpdater::update(ant, world, context);
		}
		else {
			SoldierUpdater::update(ant, world, context);
		}
	}
};
