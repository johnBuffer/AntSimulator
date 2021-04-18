#pragma once
#include <thread>
#include <mutex>
#include "world.hpp"


struct WorldRenderer
{
	const Grid<MarkerCell>& grid;
	sf::VertexArray& target_vertex_array;
	std::mutex& access_mutex;
	sf::VertexArray vertex_array;
	std::thread thread;
	bool run;

	WorldRenderer(Grid<MarkerCell>& grid_, sf::VertexArray& target, std::mutex& mutex)
		: grid(grid_)
		, target_vertex_array(target)
		, access_mutex(mutex)
		, vertex_array(sf::Quads)
		, run(true)
	{
		initializeVertexArray(vertex_array);
		thread = std::thread([this]() {update(); });
	}

	~WorldRenderer()
	{
		run = false;
		thread.join();
	}

	void initializeVertexArray(sf::VertexArray& va)
	{
		va.resize(grid.width * grid.height * 4);
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
		const sf::Vector3f to_home_color(Conf::TO_HOME_COLOR.r / 255.0f, Conf::TO_HOME_COLOR.g / 255.0f, Conf::TO_HOME_COLOR.b / 255.0f);
		const sf::Vector3f to_food_color(Conf::TO_FOOD_COLOR.r / 255.0f, Conf::TO_FOOD_COLOR.g / 255.0f, Conf::TO_FOOD_COLOR.b / 255.0f);

		uint64_t i = 0;
		const float cell_size = to<float>(grid.cell_size);
		for (int32_t x(0); x < grid.width; x++) {
			for (int32_t y(0); y < grid.height; y++) {
				const auto& cell = grid.getCst(sf::Vector2i(x, y));

				sf::Color color = Conf::FOOD_COLOR;
				if (!cell.food) {
					const float intensity_factor = 0.3f;
					const sf::Vector3f intensity_1_color = intensity_factor * to_home_color * cell.intensity[0];
					const sf::Vector3f intensity_2_color = intensity_factor * to_food_color * cell.intensity[1];
					const sf::Vector3f mixed_color(
						std::min(255.0f, intensity_1_color.x + intensity_2_color.x),
						std::min(255.0f, intensity_1_color.y + intensity_2_color.y),
						std::min(255.0f, intensity_1_color.z + intensity_2_color.z)
					);
					color = sf::Color(sf::Color(to<uint8_t>(mixed_color.x), to<uint8_t>(mixed_color.y), to<uint8_t>(mixed_color.z)));
					const float offset = 32.0f;
					vertex_array[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
					vertex_array[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
					vertex_array[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
					vertex_array[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
				}
				else {
					const float offset = 4.0f;
					vertex_array[4 * i + 0].texCoords = sf::Vector2f(100.0f + offset, offset);
					vertex_array[4 * i + 1].texCoords = sf::Vector2f(200.0f - offset, offset);
					vertex_array[4 * i + 2].texCoords = sf::Vector2f(200.0f - offset, 100.0f - offset);
					vertex_array[4 * i + 3].texCoords = sf::Vector2f(100.0f + offset, 100.0f - offset);
				}
				vertex_array[4 * i + 0].color = color;
				vertex_array[4 * i + 1].color = color;
				vertex_array[4 * i + 2].color = color;
				vertex_array[4 * i + 3].color = color;
				++i;
			}
		}

		//access_mutex.lock();
		target_vertex_array = vertex_array;
		//access_mutex.unlock();
	}
};
