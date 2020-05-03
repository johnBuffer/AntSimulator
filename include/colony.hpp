#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "ant.hpp"
#include "utils.hpp"
#include "world.hpp"


struct Colony
{
	Colony(float x, float y, uint32_t n)
		: position(x, y)
		, last_direction_update(0.0f)
	{
		for (uint32_t i(n); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI), n - i - 1);
		}
	}

	void update(const float dt, World& world)
	{		
		for (Ant& ant : ants) {
			ant.update(dt, world);
		}
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (const Ant& a : ants) {
			a.render(target, states);
		}

		sf::CircleShape circle(size);
		circle.setOrigin(size, size);
		circle.setPosition(position);
		circle.setFillColor(Conf<>::COLONY_COLOR);
		target.draw(circle, states);
	}

	const sf::Vector2f position;
	std::vector<Ant> ants;
	const float size = 20.0f;

	float last_direction_update;
	const float direction_update_period = 0.25f;

};
