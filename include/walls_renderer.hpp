#pragma once
#include <thread>
#include <mutex>
#include <iostream>
#include "double_buffer.hpp"
#include "grid.hpp"


struct WallsRenderer
{
	const GridOfNumber<uint32_t>& grid;
	DoubleObject<sf::VertexArray>& vertex_array;
	std::thread thread;
	bool run;
	std::mutex mutex;

	WallsRenderer(GridOfNumber<uint32_t>& grid_, DoubleObject<sf::VertexArray>& target)
		: grid(grid_)
		, vertex_array(target)
		, run(true)
	{
		initializeVertexArray(vertex_array.getCurrent());
		initializeVertexArray(vertex_array.getLast());
		thread = std::thread([this]() {update(); });
	}

	~WallsRenderer()
	{
		run = false;
		thread.join();
	}

	void initializeVertexArray(sf::VertexArray& va)
	{
		va = sf::VertexArray(sf::Quads, grid.width * grid.height * 4);
		uint64_t i = 0;
		const float cell_size = to<float>(grid.cell_size);
		for (int32_t x(0); x < grid.width; x++) {
			for (int32_t y(0); y < grid.height; y++) {
				const sf::Vector2f position(x * cell_size, y * cell_size);
				va[4 * i + 0].position = position;
				va[4 * i + 1].position = position + sf::Vector2f(cell_size, 0.0f);
				va[4 * i + 2].position = position + sf::Vector2f(cell_size, cell_size);
				va[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size);
				++i;
			}
		}
	}

	void update()
	{
		while (run) {
			updateVertexArray();
		}
	}

	void updateVertexArray()
	{
		sf::VertexArray& va_walls = vertex_array.getLast();
		uint64_t i = 0;
		const float cell_size = to<float>(grid.cell_size);
		for (int32_t x(0); x < grid.width; x++) {
			for (int32_t y(0); y < grid.height; y++) {
				sf::Color color(sf::Color(255, 0, 0, 0));
				if (grid.getCst(sf::Vector2i(x, y))) {
					color = sf::Color(94, 87, 87);
				}

				va_walls[4 * i + 0].color = color;
				va_walls[4 * i + 1].color = color;
				va_walls[4 * i + 2].color = color;
				va_walls[4 * i + 3].color = color;
				++i;
			}
		}

		if (mutex.try_lock()) {
			vertex_array.swap();
			mutex.unlock();
		}
	}
};
