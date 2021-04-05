#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "markers_grid.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "wall.hpp"


template<typename T>
struct Grid
{
	Grid(int32_t width_, int32_t height_, uint32_t cell_size_)
		: cell_size(cell_size_)
		, width(width_ / cell_size_)
		, height(height_ / cell_size_)
	{
		cells.resize(width * height);
	}

	T* add(const T& obj)
	{
		return add(getCellCoords(obj.position), obj);
	}

	bool isEmpty(const sf::Vector2f& position) const
	{
		const sf::Vector2i cell_coords = getCellCoords(position);

		if (checkCell(cell_coords)) {
			return cells[getIndexFromCoords(cell_coords)].empty();
		}

		return true;
	}

	std::list<T>* getAt(const sf::Vector2f& position)
	{
		const sf::Vector2i cell_coords = getCellCoords(position);

		if (checkCell(cell_coords)) {
			return &cells[getIndexFromCoords(cell_coords)];
		}

		return nullptr;
	}

	std::list<T*> getAllAt(const sf::Vector2f& position)
	{
		std::list<T*> result(0);
		const sf::Vector2i cell_coords = getCellCoords(position);
		
		for (int32_t x(-1); x < 2; ++x) {
			for (int32_t y(-1); y < 2; ++y) {
				const sf::Vector2i coords = cell_coords + sf::Vector2i(x, y);
				if (checkCell(coords)) {
					const uint64_t index = getIndexFromCoords(coords);
					for (T& m : cells[index]) {
						result.push_back(&m);
					}
				}
			}
		}

		return result;
	}

	T* add(const sf::Vector2i& cell_coords, const T& obj)
	{
		if (checkCell(cell_coords)) {
			std::list<T>& l = cells[getIndexFromCoords(cell_coords)];
			if (Conf<>::MAX_MARKERS_PER_CELL > l.size()) {
				l.emplace_back(obj);
				return &l.back();
			}
		}
		return nullptr;
	}

	bool checkCell(const sf::Vector2i& cell_coords) const
	{
		return cell_coords.x > -1 && cell_coords.x < width && cell_coords.y > -1 && cell_coords.y < height;
	}

	uint64_t getIndexFromCoords(const sf::Vector2i& cell_coords) const
	{
		return cell_coords.x + cell_coords.y * width;
	}

	sf::Vector2i getCellCoords(const sf::Vector2f& position) const
	{
		const int32_t x_cell = to<int32_t>(position.x / cell_size);
		const int32_t y_cell = to<int32_t>(position.y / cell_size);

		return sf::Vector2i(x_cell, y_cell);
	}

	std::vector<std::list<T>> cells;

	const int32_t width, height, cell_size;
};


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
					grid_walls.cells[grid_walls.getIndexFromCoords(sf::Vector2i(x, y))].emplace_back();
				}
			}
		}

		va_walls.resize(4 * grid_walls.cells.size());
		{
			uint64_t i = 0;
			const float cell_size = grid_walls.cell_size;
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
			const float cell_size = markers.cell_size;
			for (int32_t x(0); x < markers.size_width; x++) {
				for (int32_t y(0); y < markers.size_height; y++) {
					const sf::Vector2f position(x * cell_size, y * cell_size);
					va_markers[4 * i + 0].position = position;
					va_markers[4 * i + 1].position = position + sf::Vector2f(cell_size, 0.0f);
					va_markers[4 * i + 2].position = position + sf::Vector2f(cell_size, cell_size);
					va_markers[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size);

					const float offset = 30.0f;
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
		for (std::list<Food>& l : grid_food.cells) {
			l.remove_if([&](const Food& m) {
				if (m.isDone()) {
					markers.remove(m.position.x, m.position.y, Marker::Type::ToFood);
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

	void addMarker(const Marker& marker)
	{
		markers.addMarker(marker.position.x, marker.position.y, marker.type, marker.intensity, marker.permanent);
	}

	void addWall(const sf::Vector2f& position)
	{
		grid_walls.add(Wall{ position });
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		for (const std::list<Food>& l : grid_food.cells) {
			for (const Food& f : l) {
				f.render(target, states);
			}
		}

		uint64_t i = 0;
		const float cell_size = grid_walls.cell_size;
		for (int32_t x(0); x < grid_walls.width; x++) {
			for (int32_t y(0); y < grid_walls.height; y++) {
				const uint32_t index = y * grid_walls.width + x;
				sf::Color color(sf::Color(255, 0, 0, 0));
				if (grid_walls.cells[index].size()) {
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
		states.texture = &(*Conf<>::MARKER_TEXTURE);
		uint64_t i = 0;
		const float cell_size = markers.cell_size;
		for (int32_t x(0); x < markers.size_width; x++) {
			for (int32_t y(0); y < markers.size_height; y++) {
				const uint32_t index = y * markers.size_width + x;
				const auto& cell = markers.cells[index];
				const float intensity_factor = 3.0f;
				const float intensity_1 = std::min(255.0f, cell.intensity[0] * intensity_factor);
				const float intensity_2 = std::min(255.0f, cell.intensity[1] * intensity_factor);
				sf::Color color(sf::Color(intensity_1, intensity_2, 0));
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
		addMarker(Marker(sf::Vector2f(x, y), Marker::ToFood, 100000000.0f, true));
		grid_food.add(Food(x, y, 4.0f, quantity));
	}

	sf::Vector2f size;
	MarkersGrid markers;
	Grid<Wall> grid_walls;
	Grid<Food> grid_food;

	mutable sf::VertexArray va_walls;
	mutable sf::VertexArray va_markers;

	uint64_t markers_count;
};
