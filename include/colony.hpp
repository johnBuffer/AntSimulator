#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "ant.hpp"
#include "utils.hpp"


struct Colony
{
	Colony(float x, float y, uint32_t n)
		: position(x, y)
	{
		for (uint32_t i(n); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI));
		}
	}

	void update(const float dt, const Food& food)
	{
		for (Ant& a : ants) {
			a.update(dt, markers, food);
		}

		for (Marker& m : markers) {
			m.update(dt);
		}

		removeExpiredMarkers();
	}

	void render(sf::RenderTarget& target) const
	{
		for (const Ant& a : ants) {
			a.render(target);
		}

		for (const Marker& m : markers) {
			m.render(target);
		}

		sf::CircleShape circle(size);
		circle.setOrigin(size, size);
		circle.setPosition(position);
		circle.setFillColor(sf::Color::Blue);
		target.draw(circle);
	}

	void removeExpiredMarkers()
	{
		markers.remove_if([&](const Marker& m) {return m.isDone(); });
	}

	const sf::Vector2f position;
	std::vector<Ant> ants;
	std::list<Marker> markers;
	const float size = 50.0f;
};
