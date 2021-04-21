#pragma once
#include <vector>

#include "ant_mode.hpp"
#include "utils.hpp"
#include "grid.hpp"


struct MarkerCell
{
	// Stores the intensity of ToHome and ToFood markers
	float intensity[2];
	// Is the marker permanent ?
	bool permanent[2];
	// Food quantity in the cell
	uint32_t food;
	uint32_t wall;

	MarkerCell()
		: intensity{ 0.0f, 0.0f }
		, permanent{ false, false }
		, food(0)
		, wall(0)
	{}

	void update(float dt)
	{
		// Update intensities
		intensity[0] -= (!permanent[0]) * dt;
		intensity[1] -= (!permanent[1]) * dt;
		// Avoid negative values
		intensity[0] = std::max(0.0f, intensity[0]);
		intensity[1] = std::max(0.0f, intensity[1]);
		// Remove food marker if no food
		intensity[1] = intensity[1] * to<float>(!bool(!food && permanent[1]));
		permanent[1] &= to<bool>(food);
	}

	void pick()
	{
		food -= bool(food);
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

	void addFood(sf::Vector2f pos, uint32_t quantity)
	{
		MarkerCell& cell = get(pos);
		cell.food += quantity;
		cell.intensity[1] = 1.0f;
		cell.permanent[1] = true;
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

	bool isOnFood(sf::Vector2f pos) const
	{
		return getCst(pos).food;
	}

	void pickFood(sf::Vector2f pos)
	{
		get(pos).pick();
	}
};
