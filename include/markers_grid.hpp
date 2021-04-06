#pragma once
#include <vector>

#include "marker.hpp"


struct MarkersGrid
{
	struct Cell
	{
		float intensity[2];
		bool permanent[2];

		Cell()
			: intensity{0.0f, 0.0f}
			, permanent{false, false}
		{}

		void update(float dt)
		{
			// Update intensities
			intensity[0] -= (!permanent[0]) * dt;
			intensity[1] -= (!permanent[1]) * dt;
			// Avoid negative values
			intensity[0] = std::max(0.0f, intensity[0]);
			intensity[1] = std::max(0.0f, intensity[1]);
		}
	};

	uint32_t grid_width;
	uint32_t grid_height;
	uint32_t size_width;
	uint32_t size_height;
	uint32_t cell_size;
	std::vector<Cell> cells;

	MarkersGrid(uint32_t width, uint32_t height, uint32_t cell_size_)
		: grid_width(width)
		, grid_height(height)
		, size_width(width / cell_size_)
		, size_height(height / cell_size_)
		, cell_size(cell_size_)
	{
		cells.resize(size_width * size_height);
	}

	void addMarker(float x, float y, Marker::Type type, float intensity, bool permanent = false)
	{
		Cell& cell = getCellAt(x, y);
		cell.permanent[type] != permanent;
		cell.intensity[type] = std::max(cell.intensity[type], intensity);
	}

	void remove(float x, float y, Marker::Type type)
	{
		Cell& cell = getCellAt(x, y);
		cell.permanent[type] = false;
		cell.intensity[type] = 0.0f;
	}

	Cell& getCell(uint32_t x, uint32_t y)
	{
		return cells[y * size_width + x];
	}

	Cell& getCellAt(float x, float y)
	{
		const uint32_t cell_x = x / cell_size;
		const uint32_t cell_y = y / cell_size;
		return getCell(cell_x, cell_y);
	}

	void update(float dt)
	{
		for (Cell& c : cells) {
			c.update(dt);
		}
	}
};
