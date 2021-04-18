#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "markers_grid.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "wall.hpp"
#include "grid.hpp"
#include "ant_mode.hpp"
#include "markers_renderer.hpp"


struct World
{
	World(uint32_t width, uint32_t height)
		: markers(width, height, 4)
		, grid_walls(width, height, 10)
		, size(to<float>(width), to<float>(height))
		, va_walls(sf::Quads)
		, va_markers(sf::Quads)
		, renderer(markers, va_markers, mutex)
	{
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				if (x == 0 || x == grid_walls.width - 1 || y == 0 || y == grid_walls.height - 1) {
					grid_walls.get(sf::Vector2i(x, y)) = 1;
				}
			}
		}

		va_walls.resize(4 * grid_walls.cells.size());
		{
			uint64_t i = 0;
			const float cell_size = to<float>(grid_walls.cell_size);
			for (int32_t x(0); x < grid_walls.width; x++) {
				for (int32_t y(0); y < grid_walls.height; y++) {
					sf::Vector2f position(x * cell_size, y * cell_size);
					va_walls[4 * i + 0].position = position;
					va_walls[4 * i + 1].position = position + sf::Vector2f(cell_size, 0.0f);
					va_walls[4 * i + 2].position = position + sf::Vector2f(cell_size, cell_size);
					va_walls[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size);
					++i;
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

	void renderWalls(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		uint64_t i = 0;
		const float cell_size = to<float>(grid_walls.cell_size);
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				const uint32_t index = y * grid_walls.width + x;
				sf::Color color(sf::Color(255, 0, 0, 0));
				if (grid_walls.cells[index]) {
					color = sf::Color(94, 87, 87);
				}

				va_walls[4 * i + 0].color = color;
				va_walls[4 * i + 1].color = color;
				va_walls[4 * i + 2].color = color;
				va_walls[4 * i + 3].color = color;
				++i;
			}
		}

		target.draw(va_walls, states);
	}

	void renderMarkers(sf::RenderTarget& target, sf::RenderStates states)
	{
		states.texture = &(*Conf::MARKER_TEXTURE);
		//mutex.lock();
		target.draw(va_markers, states);
		//mutex.unlock();
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

	mutable sf::VertexArray va_walls;
	mutable sf::VertexArray va_markers;
	std::mutex mutex;
	WorldRenderer renderer;
};
