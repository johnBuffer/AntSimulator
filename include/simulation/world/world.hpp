#pragma once
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
	DoubleObject<sf::VertexArray> va_map;
	WorldRenderer renderer;

	World(uint32_t width, uint32_t height)
		: map(width, height, 4)
		, size(to<float>(width), to<float>(height))
		, renderer(map, va_map)
	{
        // Create walls around the map
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

	void addMarkerRepellent(sf::Vector2f pos, uint64_t colony_id, float amount)
	{
		map.get(pos).markers[colony_id].repellent += amount;
	}

	void addWall(const sf::Vector2f& position)
	{
		addWall(sf::Vector2i{to<int32_t>(position.x) / map.cell_size, to<int32_t>(position.y) / map.cell_size});
	}

    void addWall(const sf::Vector2i& position)
    {
        if (map.checkCoords(position)) {
			WorldCell& cell = map.get(position);
            cell.food = 0;
            cell.wall = 1;
			for (auto& markers : cell.markers) {
				clearMarkersOfCell(markers);
			}
        }
    }

	void removeWall(const sf::Vector2f& position)
	{
		if (map.checkCoords(position)) {
			map.get(position).wall = 0;
		}
	}

	void render(sf::RenderTarget& target, sf::RenderStates states)
	{
		states.texture = &(*Conf::MARKER_TEXTURE);
		renderer.mutex.lock();
		target.draw(va_map.getCurrent(), states);
		renderer.mutex.unlock();
	}

	void addFoodAt(float x, float y, uint32_t quantity)
	{
        addFoodAt(sf::Vector2i{to<int32_t>(x) / map.cell_size, to<int32_t>(y) / map.cell_size}, quantity);
	}

    void addFoodAt(sf::Vector2i pos, uint32_t quantity)
    {
        if (map.checkCoords(pos)) {
            map.addMarker(pos, Mode::ToFood, 1.0f, true);
            map.addFood(pos, quantity);
        }
    }

    void clearMarkers(uint8_t colony_id)
    {
        for (auto& cell : map.cells) {
			clearMarkersOfCell(cell.markers[colony_id]);
            cell.density = 0.0f;
        }
    }

	static void clearMarkersOfCell(ColonyCell& cell)
	{
		cell.intensity[0] = 0.0f;
		cell.intensity[1] = 0.0f;
		cell.intensity[2] = 0.0f;
		cell.repellent    = 0.0f;
		cell.permanent    = false;
	}
};
