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
	{
		for (uint32_t i(n); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI), n - i - 1);
		}
	}

	void update(const float dt, World& world)
	{
		for (Ant& a : ants) {
			a.update(dt, world);
		}
	}

	void render(sf::RenderTarget& target) const
	{
		for (const Ant& a : ants) {
			a.render(target);
		}

		sf::CircleShape circle(size);
		circle.setOrigin(size, size);
		circle.setPosition(position);
		circle.setFillColor(Conf<>::COLONY_COLOR);
		target.draw(circle);
	}

	const sf::Vector2f position;
	std::vector<Ant> ants;
	const float size = 20.0f;
};
