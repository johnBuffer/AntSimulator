#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "world_grid.hpp"
#include "utils.hpp"
#include "wall.hpp"
#include "grid.hpp"
#include "ant_mode.hpp"
#include "world_renderer.hpp"


struct World
{
	sf::Vector2f size;
	WorldGrid map;
	DoubleObject<sf::VertexArray> va_map;
	DoubleObject<sf::VertexArray> va_walls;
	WorldRenderer renderer;

	World(uint32_t width, uint32_t height)
		: map(width, height, 4)
		, size(to<float>(width), to<float>(height))
		, renderer(map, va_map)
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

	void addMarker(sf::Vector2f pos, Mode type, double intensity, bool permanent = false)
	{
		map.addMarker(pos, type, intensity, permanent);
	}

	void addMarkerRepellent(sf::Vector2f pos, float amount)
	{
		map.get(pos).repellent += amount;
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

	void renderMap(sf::RenderTarget& target, sf::RenderStates states)
	{
		states.texture = &(*Conf::MARKER_TEXTURE);
		renderer.mutex.lock();
		target.draw(va_map.getCurrent(), states);
		renderer.mutex.unlock();
	}

	void addFoodAt(float x, float y, uint32_t quantity)
	{
		const sf::Vector2f pos(x, y);
		if (map.checkCoords(pos)) {
			addMarker(pos, Mode::ToFood, 100000000.0f, true);
			map.addFood(pos, quantity);
		}
	}
};
