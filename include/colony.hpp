#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "ant.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "colony_base.hpp"
#include "graph.hpp"


struct Colony
{
	ColonyBase base;
	uint32_t max_ants_count;
	std::vector<Ant> ants;
	mutable sf::VertexArray ants_va;
	Cooldown ants_creation_cooldown;
	Graphic population;
	Cooldown population_update;

	Colony(float x, float y, uint32_t n)
		: base(sf::Vector2f(x, y), 20.0f)
		, max_ants_count(n)
		, ants_va(sf::Quads, 4 * n)
		, ants_creation_cooldown(10.0f)
		, population(800, sf::Vector2f(800.0f, 100.0f), sf::Vector2f())
		, population_update(3.0f)
	{
		base.food = 9.0f;
		uint32_t ants_count = 5000;
		for (uint32_t i(ants_count); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI));
		}

		for (uint64_t i(0); i < n; ++i) {
			const uint64_t index = 4 * i;
			ants_va[index + 0].color = Conf::ANT_COLOR;
			ants_va[index + 1].color = Conf::ANT_COLOR;
			ants_va[index + 2].color = Conf::ANT_COLOR;
			ants_va[index + 3].color = Conf::ANT_COLOR;

			ants_va[index + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
			ants_va[index + 1].texCoords = sf::Vector2f(73.0f, 0.0f);
			ants_va[index + 2].texCoords = sf::Vector2f(73.0f, 107.0f);
			ants_va[index + 3].texCoords = sf::Vector2f(0.0f, 107.0f);
		}
	}

	void update(float dt, World& world)
	{
		const float ant_cost = 5.0f;
		ants_creation_cooldown.update(dt);
		if (ants_creation_cooldown.ready() && ants.size() < max_ants_count && base.useFood(ant_cost)) {
			ants.emplace_back(base.position.x, base.position.y, getRandRange(2.0f * PI));
		}
		
		for (Ant& ant : ants) {
			ant.update(dt, world);
		}

		for (Ant& ant : ants) {
			ant.checkColony(base);
		}

		// Remove dead ants
		auto it = std::remove_if(ants.begin(), ants.end(), [](const Ant& a) { return a.autonomy > a.max_autonomy; });
		ants.erase(it, ants.end());

		population.setLastValue(to<float>(ants.size()));
		population_update.update(dt);
		if (population_update.ready()) {
			population_update.reset();
			population.next();
		}
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (const Ant& a : ants) {
			a.render_food(target, states);
		}

		uint32_t index = 0;
		for (const Ant& a : ants) {
			a.render_in(ants_va, 4 * (index++));
		}

		const Ant placeholder(base.position.x, base.position.y, 0.0f);
		for (uint32_t i(index); i < max_ants_count; ++i) {
			placeholder.render_in(ants_va, 4 * i);
		}

		sf::RenderStates rs = states;
		rs.texture = &(*Conf::ANT_TEXTURE);
		target.draw(ants_va, rs);
	}
};
