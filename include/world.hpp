#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "markers_grid.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "wall.hpp"
#include "grid.hpp"
#include "ant_mode.hpp"



struct World
{
	World(uint32_t width, uint32_t height)
		: markers(width, height, 4)
		, grid_walls(width, height, 10)
		, grid_food(width, height, 5)
		, size(to<float>(width), to<float>(height))
		, va_walls(sf::Quads)
		, va_markers(sf::Quads)
	{
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				if (x == 0 || x == grid_walls.width - 1 || y == 0 || y == grid_walls.height - 1) {
					grid_walls.emplaceAtPosition(sf::Vector2i(x, y));
				}
			}
		}

		va_walls.resize(4 * grid_walls.cells.size());
		{
			uint64_t i = 0;
			const float cell_size = to<float>(grid_walls.cell_size);
			for (int32_t x(0); x < grid_walls.width; x++) {
				for (int32_t y(0); y < grid_walls.height; y++) {
					sf::Vector2f position(x * cell_size, y * cell_size);
					va_walls[4 * i + 0].position = position;
					va_walls[4 * i + 1].position = position + sf::Vector2f(cell_size, 0.0f);
					va_walls[4 * i + 2].position = position + sf::Vector2f(cell_size, cell_size);
					va_walls[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size);
					++i;
				}
			}
		}

		va_markers.resize(4 * markers.cells.size());
		{
			uint64_t i = 0;
			const float cell_size = to<float>(markers.cell_size);
			for (int32_t x(0); x < markers.width; x++) {
				for (int32_t y(0); y < markers.height; y++) {
					const sf::Vector2f position(x * cell_size, y * cell_size);
					va_markers[4 * i + 0].position = position;
					va_markers[4 * i + 1].position = position + sf::Vector2f(cell_size, 0.0f);
					va_markers[4 * i + 2].position = position + sf::Vector2f(cell_size, cell_size);
					va_markers[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size);
					const float offset = 32.0f;
					va_markers[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
					va_markers[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
					va_markers[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
					va_markers[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
					++i;
				}
			}
		}
	}

	void removeExpiredFood()
	{
		for (GridListCell<Food>& c : grid_food.cells) {
			c.data.remove_if([&](const Food& m) {
				if (m.isDone()) {
					markers.remove(m.position, Mode::ToFood);
					return true;
				}
				return false;
			});
		}
	}

	void update(float dt)
	{
		removeExpiredFood();
		markers.update(dt);
	}

	void addMarker(sf::Vector2f pos, Mode type, float intensity, bool permanent = false)
	{
		markers.addMarker(pos, type, intensity, permanent);
	}

	void addWall(const sf::Vector2f& position)
	{
		grid_walls.add(Wall{ position });
	}

	void removeWall(const sf::Vector2f& position)
	{
		grid_walls.clearAt(position);
	}

	void renderFood(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (const GridListCell<Food>& c : grid_food.cells) {
			for (const Food& f : c.data) {
				f.render(target, states);
			}
		}
	}

	void renderWalls(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		uint64_t i = 0;
		const float cell_size = to<float>(grid_walls.cell_size);
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				const uint32_t index = y * grid_walls.width + x;
				sf::Color color(sf::Color(255, 0, 0, 0));
				if (grid_walls.cells[index].data.size()) {
					color = sf::Color(94, 87, 87);
				}

				va_walls[4 * i + 0].color = color;
				va_walls[4 * i + 1].color = color;
				va_walls[4 * i + 2].color = color;
				va_walls[4 * i + 3].color = color;
				++i;
			}
		}

		target.draw(va_walls, states);
	}

	void renderMarkers(sf::RenderTarget& target, sf::RenderStates states) const
	{
		const sf::Vector3f to_home_color(Conf::TO_HOME_COLOR.r / 255.0f, Conf::TO_HOME_COLOR.g / 255.0f, Conf::TO_HOME_COLOR.b / 255.0f);
		const sf::Vector3f to_food_color(Conf::TO_FOOD_COLOR.r / 255.0f, Conf::TO_FOOD_COLOR.g / 255.0f, Conf::TO_FOOD_COLOR.b / 255.0f);

		states.texture = &(*Conf::MARKER_TEXTURE);
		uint64_t i = 0;
		const float cell_size = to<float>(markers.cell_size);
		for (int32_t x(0); x < markers.width; x++) {
			for (int32_t y(0); y < markers.height; y++) {
				const uint64_t index = markers.getIndexFromCoords(sf::Vector2i(x, y));
				const auto& cell = markers.cells[index];
				const float intensity_factor = 0.3f;
				const sf::Vector3f intensity_1_color = intensity_factor * to_home_color * cell.intensity[0];
				const sf::Vector3f intensity_2_color = intensity_factor * to_food_color * cell.intensity[1];
				const sf::Vector3f mixed_color(
					std::min(255.0f, intensity_1_color.x + intensity_2_color.x),
					std::min(255.0f, intensity_1_color.y + intensity_2_color.y),
					std::min(255.0f, intensity_1_color.z + intensity_2_color.z)
				);
				sf::Color color(sf::Color(to<uint8_t>(mixed_color.x), to<uint8_t>(mixed_color.y), to<uint8_t>(mixed_color.z)));
				const sf::Vector2f position(x * cell_size, y * cell_size);
				va_markers[4 * i + 0].color = color;
				va_markers[4 * i + 1].color = color;
				va_markers[4 * i + 2].color = color;
				va_markers[4 * i + 3].color = color;
				++i;
			}
		}

		target.draw(va_markers, states);
	}

	void addFoodAt(float x, float y, float quantity)
	{
		addMarker(sf::Vector2f(x, y), Mode::ToFood, 100000000.0f, true);
		grid_food.add(Food(x, y, 4.0f, quantity));
	}

	sf::Vector2f size;
	MarkersGrid markers;
	GridOfList<Wall> grid_walls;
	GridOfList<Food> grid_food;

	mutable sf::VertexArray va_walls;
	mutable sf::VertexArray va_markers;

	uint64_t markers_count;
};
