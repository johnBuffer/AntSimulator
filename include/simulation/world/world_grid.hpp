#pragma once
#include <vector>

#include "simulation/ant/ant_mode.hpp"
#include "common/utils.hpp"
#include "common/grid.hpp"


constexpr uint8_t max_colonies_count = 2;
constexpr float min_intensity = 0.1f;


struct AntRef
{
	bool     active = false;
	uint8_t  col_id;
	uint16_t ant_id;
};


struct ColonyCell
{
	// Stores the intensity of ToHome and ToFood markers
	float intensity[3];
	// Is the marker permanent ?
	bool permanent = false;
	// Repellent instensity
	float repellent;
	// Current ant
	int16_t current_ant = -1;
    bool fighting;

	ColonyCell()
		: intensity{ min_intensity, min_intensity, min_intensity }
		, repellent(0.0f)
        , fighting(false)
	{}

	void update(float dt)
	{
		// Reset current ant
		current_ant = -1;
        fighting = false;
		// Update toFood and toHome
		intensity[0] -= permanent ? dt : 0.0f;
		intensity[1] -= dt;
		intensity[2] -= dt;
		// Update repellents
		repellent -= dt;
		repellent = std::max(0.0f, repellent);
	}
};


struct WorldCell
{
	ColonyCell markers[max_colonies_count];
	// Food quantity in the cell
	uint32_t food;
	uint32_t wall;
	// Density of ants a a certain point
	float density;
	// Dist to wall
	float wall_dist;
	float discovered;

	WorldCell()
		: food(0)
		, wall(0)
		, density(0.0f)
		, wall_dist(0.0f)
		, discovered(1.0f)
	{
		for (ColonyCell& c : markers) {
			c = ColonyCell();
		}
	}

	void update(float dt)
	{
		// Update intensities
		for (uint8_t i(max_colonies_count); i--;) {
			markers[i].update(dt);
		}
		// Update density
		density *= 0.99f;
		//discovered += dt * float(bool(discovered));
	}

	bool pick()
	{
		const bool last = (food <= 1.0f);
		food = (food - bool(food)) * (!last);
		return last;
	}
    
    AntRef getEnemy(uint8_t team)
    {
		AntRef res;
        // Update intensities
        for (uint8_t i(max_colonies_count); i--;) {
            if (i != team && markers[i].current_ant > -1) {
				res.active = true;
				res.ant_id = markers[i].current_ant;
				res.col_id = i;
                return res;
            }
        }
        return res;
    }

	bool checkEnemyPresence(uint8_t team)
	{
		// Update intensities
		for (uint8_t i(0); i<max_colonies_count; ++i) {
			if (i != team && markers[i].current_ant > -1) {
				return true;
			}
		}
		return false;
	}

	bool checkFight(uint8_t colony_id)
	{
		return markers[colony_id].fighting;
	}

	float& getRepellent(uint8_t colony_id)
	{
		return markers[colony_id].repellent;
	}

	float getIntensity(Mode mode, uint8_t colony_id) const
	{
		return std::max(min_intensity, markers[colony_id].intensity[to<uint32_t>(mode)]);
	}

	bool isPermanent(uint8_t colony_id) const
	{
		return markers[colony_id].permanent;
	}

	void degrade(uint8_t colony_id, Mode mode, float ratio)
	{
		markers[colony_id].intensity[to<uint32_t>(mode)] *= ratio;
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

	void addMarker(sf::Vector2f pos, Mode type, float intensity, uint8_t colony_id, bool permanent = false)
	{
		ColonyCell& cell = get(pos).markers[colony_id];
		const uint32_t mode_index = to<uint32_t>(type);
		// Overwrite intensity if different colony
		cell.intensity[mode_index] = std::max(cell.intensity[mode_index], intensity);
		cell.permanent |= permanent;
	}

	void addFood(sf::Vector2f pos, uint32_t quantity)
	{
		WorldCell& cell = get(pos);
		if (!cell.wall) {
			cell.food += quantity;
		}
	}

	void remove(sf::Vector2f pos, Mode type, uint8_t colony_id)
	{
		WorldCell& cell = get(pos);
		const uint32_t mode_index = to<uint32_t>(type);
		cell.markers[colony_id].intensity[mode_index] = 0.0f;
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
