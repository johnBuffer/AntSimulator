#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "marker.hpp"
#include "food.hpp"


struct Grid
{
	Grid(uint32_t width_, uint32_t height_, uint32_t cell_size_)
		: cell_size(cell_size_)
		, width(width_ / cell_size_)
		, height(height_ / cell_size_)
	{
		cells.resize(width * height);
	}

	void addMarker(const Marker& marker)
	{
		addMarker(getCellCoords(marker.position), marker);
	}

	std::list<Marker>* getMarkersAt(const sf::Vector2f& position)
	{
		const sf::Vector2i cell_coords = getCellCoords(position);

		if (checkCell(cell_coords)) {
			return &cells[getIndexFromCoords(cell_coords)];
		}

		return nullptr;
	}

	void addMarker(const sf::Vector2i& cell_coords, const Marker& marker)
	{
		if (checkCell(cell_coords)) {
			cells[getIndexFromCoords(cell_coords)].emplace_back(marker);
		}
	}

	bool checkCell(const sf::Vector2i& cell_coords)
	{
		return cell_coords.x > -1 && cell_coords.x < width && cell_coords.y > -1 && cell_coords.y < height;
	}

	uint64_t getIndexFromCoords(const sf::Vector2i& cell_coords)
	{
		return cell_coords.x + cell_coords.y * width;
	}

	sf::Vector2i getCellCoords(const sf::Vector2f& position)
	{
		const int32_t x_cell = position.x / cell_size;
		const int32_t y_cell = position.y / cell_size;

		return sf::Vector2i(x_cell, y_cell);
	}

	std::vector<std::list<Marker>> cells;

	const uint32_t width, height, cell_size;
};


struct World
{
	World(uint32_t width, uint32_t height)
		: grid(width, height, 150)
	{}

	void removeExpiredMarkers()
	{
		for (std::list<Marker>& l : grid.cells) {
			l.remove_if([&](const Marker& m) {return m.isDone(); });
		}
	}

	void update(const float dt)
	{
		removeExpiredMarkers();
		for (std::list<Marker>& l : grid.cells) {
			for (Marker& m : l) {
				m.update(dt);
			}
		}
	}

	void render(sf::RenderTarget& target) const
	{
		for (const std::list<Marker>& l : grid.cells) {
			for (const Marker& m : l) {
				m.render(target);
			}
		}

		food.render(target);
	}

	Grid grid;
	Food food;
};
