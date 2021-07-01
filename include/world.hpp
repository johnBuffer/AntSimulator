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
	WorldGrid markers;
	DoubleObject<sf::VertexArray> va_markers;
	DoubleObject<sf::VertexArray> va_walls;
	WorldRenderer renderer;

	World(uint32_t width, uint32_t height)
		: size(to<float>(width), to<float>(height))
		, markers(width, height, 4)
		, renderer(markers, va_markers)
	{
		for (int32_t x(0); x < markers.width; x++) {
			for (int32_t y(0); y < markers.height; y++) {
				if (x == 0 || x == markers.width - 1 || y == 0 || y == markers.height - 1) {
					markers.get(sf::Vector2i(x, y)).wall = 1;
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
		if (markers.checkCoords(position)) {
			markers.get(position).wall = 1;
		}
	}

	void removeWall(const sf::Vector2f& position)
	{
		if (markers.checkCoords(position)) {
			markers.get(position).wall = 0;
		}
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
		if (markers.checkCoords(pos)) {
			addMarker(pos, Mode::ToFood, 1.0f, true);
			markers.addFood(pos, quantity);
		}
	}
};
