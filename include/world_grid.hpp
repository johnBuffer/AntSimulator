#pragma once
#include <vector>

#include "ant_mode.hpp"
#include "utils.hpp"
#include "grid.hpp"


struct WorldCell
{
	// Stores the intensity of ToHome and ToFood markers
	double intensity[2];
	// Is the marker permanent ?
	bool permanent[2];
	// Food quantity in the cell
	uint32_t food;
	uint32_t wall;
	// Repellent
	float repellent;
	// Density of ants a a certain point
	float density;
	// Dist to wall
	float wall_dist;
	float discovered;

	WorldCell()
		: intensity{ 0.0f, 0.0f }
		, permanent{ false, false }
		, food(0)
		, wall(0)
		, repellent(0.0f)
		, density(0.0f)
		, wall_dist(0.0f)
		, discovered(0.0f)
	{}

	void update(float dt)
	{
		// Update intensities
		intensity[0] -= (!permanent[0]) * dt;
		intensity[1] -= (!permanent[1]) * dt;
		// Avoid negative values
		intensity[0] = std::max(0.1, intensity[0]);
		intensity[1] = std::max(0.1, intensity[1]);
		// Remove food marker if no food
		intensity[1] = intensity[1] * to<float>(!bool(!food && permanent[1]));
		permanent[1] &= to<bool>(food);
		// Update repellents
		repellent -= dt;
		repellent = std::max(0.0f, repellent);
		// Update density
		density *= 0.99f;
		discovered += dt * float(bool(discovered));
	}

	bool pick()
	{
		const bool last = (food <= 1.0f);
		food = (food - bool(food)) * float(!last);
		return last;
	}

	double getIntensity(Mode mode) const
	{
		return intensity[to<uint32_t>(mode)];
	}

	bool isPermanent(Mode mode) const
	{
		return permanent[to<uint32_t>(mode)];
	}

	void degrade()
	{
		intensity[to<uint32_t>(Mode::ToFood)] *= 0.25f;
	}

	void addPresence()
	{
		density += 1.0f;
	}
};


struct HitPoint
{
	WorldCell* cell;
	sf::Vector2f normal;
	float distance;

	HitPoint()
		: cell(nullptr)
		, distance(-1.0f)
	{}

	HitPoint(WorldCell& c, sf::Vector2f n, float dist)
		: cell(&c)
		, normal(n)
		, distance(dist)
	{}

	bool isDistanceUnder(float d) const
	{
		return (distance != -1.0f) && (distance < d);
	}
};


struct WorldGrid : public Grid<WorldCell>
{
	WorldGrid(uint32_t width_, uint32_t height_, uint32_t cell_size_)
		: Grid(width_, height_, cell_size_)
	{
	}

	void addMarker(sf::Vector2f pos, Mode type, double intensity, bool permanent = false)
	{
		WorldCell& cell = get(pos);
		const uint32_t mode_index = to<uint32_t>(type);
		cell.permanent[mode_index] |= permanent;
		cell.intensity[mode_index] = std::max(cell.intensity[mode_index], intensity);
	}

	void addFood(sf::Vector2f pos, uint32_t quantity)
	{
		WorldCell& cell = get(pos);
		cell.food += quantity;
		cell.intensity[1] = 1.0f;
		cell.permanent[1] = true;
	}

	void remove(sf::Vector2f pos, Mode type)
	{
		WorldCell& cell = get(pos);
		const uint32_t mode_index = to<uint32_t>(type);
		cell.permanent[mode_index] = false;
		cell.intensity[mode_index] = 0.0f;
	}

	void update(float dt)
	{
		for (WorldCell& c : cells) {
			c.update(dt);
		}
	}

	bool isOnFood(sf::Vector2f pos) const
	{
		return getCst(pos).food;
	}

	bool pickFood(sf::Vector2f pos)
	{
		return get(pos).pick();
	}

	HitPoint getFirstHit(sf::Vector2f p, sf::Vector2f d, float max_dist)
	{
		HitPoint intersection;
		sf::Vector2i cell_p = getCellCoords(p);
		const sf::Vector2i step(d.x < 0.0f ? -1 : 1, d.y < 0.0f ? -1 : 1);
		const sf::Vector2f inv_d(1.0f / d.x, 1.0f / d.y);
		const float t_dx = std::abs(float(cell_size) * inv_d.x);
		const float t_dy = std::abs(float(cell_size) * inv_d.y);
		float t_max_x = ((cell_p.x + (step.x > 0)) * cell_size - p.x) * inv_d.x;
		float t_max_y = ((cell_p.y + (step.y > 0)) * cell_size - p.y) * inv_d.y;
		float dist = 0.0f;
		while (dist < max_dist) {
			const uint32_t b = t_max_x < t_max_y;
			// Advance in grid
			dist = (b * t_max_x + (!b) * t_max_y);
			t_max_x += t_dx * b;
			t_max_y += t_dy * (!b);
			cell_p.x += step.x * b;
			cell_p.y += step.y * (!b);
			if (!checkCoords(cell_p)) {
				return intersection;
			}
			else {
				WorldCell& cell = get(cell_p);
				if (cell.wall) {
					intersection.cell = &cell;
					intersection.normal = sf::Vector2f(to<float>(b), to<float>(!b));
					intersection.distance = dist;
					return intersection;
				}
			}
		}
		return intersection;
	}
};
