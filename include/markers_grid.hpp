#pragma once
#include <vector>

#include "ant_mode.hpp"
#include "utils.hpp"
#include "grid.hpp"


struct MarkerCell
{
	float intensity[2];
	bool permanent[2];

	MarkerCell()
		: intensity{ 0.0f, 0.0f }
		, permanent{ false, false }
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


struct MarkersGrid : public Grid<MarkerCell>
{
	MarkersGrid(uint32_t width_, uint32_t height_, uint32_t cell_size_)
		: Grid(width_, height_, cell_size_)
	{
	}

	void addMarker(sf::Vector2f pos, Mode type, float intensity, bool permanent = false)
	{
		MarkerCell& cell = get(pos);
		const uint32_t mode_index = to<uint32_t>(type);
		cell.permanent[mode_index] |= permanent;
		cell.intensity[mode_index] = std::max(cell.intensity[mode_index], intensity);
	}

	void remove(sf::Vector2f pos, Mode type)
	{
		MarkerCell& cell = get(pos);
		const uint32_t mode_index = to<uint32_t>(type);
		cell.permanent[mode_index] = false;
		cell.intensity[mode_index] = 0.0f;
	}

	void update(float dt)
	{
		for (MarkerCell& c : cells) {
			c.update(dt);
		}
	}
};
