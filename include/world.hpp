#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "markers_grid.hpp"
#include "utils.hpp"
#include "wall.hpp"
#include "grid.hpp"
#include "ant_mode.hpp"
#include "markers_renderer.hpp"
#include "walls_renderer.hpp"


struct World
{
	World(uint32_t width, uint32_t height)
		: markers(width, height, 4)
		, grid_walls(width, height, 10)
		, size(to<float>(width), to<float>(height))
		, renderer(markers, va_markers)
		, walls_renderer(grid_walls, va_walls)
	{
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				if (x == 0 || x == grid_walls.width - 1 || y == 0 || y == grid_walls.height - 1) {
					grid_walls.get(sf::Vector2i(x, y)) = 1;
				}
			}
		}
	}

	void update(float dt)
	{
		markers.update(dt);
	}

	void addMarker(sf::Vector2f pos, Mode type, float intensity, bool permanent = false)
	{
		markers.addMarker(pos, type, intensity, permanent);
	}

	void addWall(const sf::Vector2f& position)
	{
		grid_walls.get(position) = 1;
	}

	void removeWall(const sf::Vector2f& position)
	{
		grid_walls.get(position) = 0;
	}

	void renderWalls(sf::RenderTarget& target, const sf::RenderStates& states)
	{
		walls_renderer.mutex.lock();
		target.draw(va_walls.getCurrent(), states);
		walls_renderer.mutex.unlock();
	}

	void renderMarkers(sf::RenderTarget& target, sf::RenderStates states)
	{
		states.texture = &(*Conf::MARKER_TEXTURE);
		renderer.mutex.lock();
		target.draw(va_markers.getCurrent(), states);
		renderer.mutex.unlock();
	}

	void addFoodAt(float x, float y, uint32_t quantity)
	{
		const sf::Vector2f pos(x, y);
		addMarker(pos, Mode::ToFood, 100000000.0f, true);
		markers.addFood(pos, quantity);
	}

	sf::Vector2f size;
	MarkersGrid markers;
	GridOfNumber<uint32_t> grid_walls;

	DoubleObject<sf::VertexArray> va_markers;
	DoubleObject<sf::VertexArray> va_walls;
	WorldRenderer renderer;
	WallsRenderer walls_renderer;
};
