#pragma once
#include <vector>
#include <list>
#include <SFML/System.hpp>


template<typename T>
struct Grid
{
	std::vector<T> cells;

	const int32_t width, height, cell_size;

	Grid(int32_t width_, int32_t height_, uint32_t cell_size_)
		: cell_size(cell_size_)
		, width(width_ / cell_size_)
		, height(height_ / cell_size_)
	{
		cells.resize(width * height);
	}

	T* getSafe(sf::Vector2f pos)
	{
		sf::Vector2i cell_coords = getCellCoords(pos);
		if (checkCoords(cell_coords)) {
			return &get(cell_coords);
		}
		return nullptr;
	}

	const T& getCst(sf::Vector2i cell_coord) const
	{
		return cells[getIndexFromCoords(cell_coord)];
	}

	const T& getCst(sf::Vector2f position) const
	{
		return getCst(getCellCoords(position));
	}

	T& get(sf::Vector2f position)
	{
		return const_cast<T&>(getCst(position));
	}

	T& get(sf::Vector2i cell_coord)
	{
		return const_cast<T&>(getCst(cell_coord));
	}

	sf::Vector2f getCellCenter(sf::Vector2f position)
	{
		const sf::Vector2i cell_coords = getCellCoords(position);
		return float(cell_size) * sf::Vector2f(cell_coords.x + 0.5f, cell_coords.y + 0.5f);
	}

	bool checkCoords(const sf::Vector2f& position) const
	{
		return checkCoords(getCellCoords(position));
	}

	bool checkCoords(const sf::Vector2i& cell_coords) const
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
};
