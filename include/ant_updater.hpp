#pragma once
#include "ant.hpp"
#include "world.hpp"


struct SoldierUpdater
{
	static void update(Ant& ant, World& world)
	{

	}
};


struct WorkerUpdater
{
	static void update(Ant& ant, World& world, float dt)
	{
		ant.autonomy += dt;
		// Update current direction
		ant.direction.update(dt);
		// Add ant to current cell
		ant.addToWorldGrid(world);
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
		if (ant.autonomy > 0.75f * ant.max_autonomy) {
			ant.phase = Mode::Refill;
		}
		// Check collision with food
		if (ant.phase == Mode::ToFood) {
			ant.checkFood(world);
		}
		// Get current cell
		WorldCell& cell = world.map.get(ant.position);
		cell.addPresence();
		ant.search_markers.update(dt);
		ant.direction_update.update(dt);
		if (ant.direction_update.ready()) {
			ant.direction_update.reset();
			if (ant.search_markers.ready()) {
				ant.findMarker(world);
				ant.direction += RNGf::getFullRange(ant.direction_noise_range);
			}
			else {
				cell.degrade(ant.col_id, Mode::ToFood, 0.25f);
				ant.direction += RNGf::getFullRange(2.0f * ant.direction_noise_range);
			}
		}
		// Add marker
		ant.marker_add.update(dt);
		if (ant.marker_add.ready()) {
			ant.addMarker(world);
			ant.marker_add.reset();
		}
	}
};


struct AntUpdater
{
	static void updateAnt(Ant& ant, World& world)
	{

	}
};
