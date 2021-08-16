#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "world_grid.hpp"
#include "common/utils.hpp"
#include "wall.hpp"
#include "common/grid.hpp"
#include "simulation/ant/ant_mode.hpp"
#include "render/world_renderer.hpp"


struct World
{
	sf::Vector2f size;
	WorldGrid map;

	World(uint32_t width, uint32_t height)
		: map(width, height, 4)
		, size(to<float>(width), to<float>(height))
	{
		for (int32_t x(0); x < map.width; x++) {
			for (int32_t y(0); y < map.height; y++) {
				if (x == 0 || x == map.width - 1 || y == 0 || y == map.height - 1) {
					map.get(sf::Vector2i(x, y)).wall = 1;
				}
			}
		}
	}

	void update(float dt)
	{
		map.update(dt);
	}

	void addMarker(sf::Vector2f pos, Mode type, float intensity, uint8_t colony_id, bool permanent = false)
	{
		map.addMarker(pos, type, intensity, colony_id, permanent);
	}

	void addMarkerRepellent(sf::Vector2f pos, uint8_t colony_id, float amount)
	{
		map.get(pos).markers[colony_id].repellent += amount;
	}

	void addWall(const sf::Vector2f& position)
	{
		if (map.checkCoords(position)) {
			map.get(position).wall = 1;
		}
	}

	void removeWall(const sf::Vector2f& position)
	{
		if (map.checkCoords(position)) {
			map.get(position).wall = 0;
		}
	}

	void addFoodAt(float x, float y, uint32_t quantity)
	{
		const sf::Vector2f pos(x, y);
		if (map.checkCoords(pos)) {
			addMarker(pos, Mode::ToFood, 1.0f, true);
			map.addFood(pos, quantity);
		}
	}
};
