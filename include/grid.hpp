#pragma once
#include <vector>
#include <list>


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

	sf::Vector2f getCellCenter(sf::Vector2f position)
	{
		const sf::Vector2i cell_coords = getCellCoords(position);
		return float(cell_size) * sf::Vector2f(cell_coords.x + 0.5f, cell_coords.y + 0.5f);
	}

	bool checkCell(const sf::Vector2i& cell_coords) const
	{
		return cell_coords.x > -1 && cell_coords.x < width&& cell_coords.y > -1 && cell_coords.y < height;
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


template<typename T>
struct GridListCell
{
	bool empty;
	std::list<T> data;
};



template<typename T>
struct GridOfList : public Grid<GridListCell<T>>
{
	GridOfList(int32_t width_, int32_t height_, uint32_t cell_size_)
		: Grid(width_, height_, cell_size)
	{
	}

	T* add(const T& obj)
	{
		return add(getCellCoords(obj.position), obj);
	}

	void clearAt(sf::Vector2f position)
	{
		auto cell_coords = getCellCoords(position);
		cells[getIndexFromCoords(cell_coords)].clear();
	}

	T* add(const sf::Vector2i& cell_coords, const T& obj)
	{
		if (checkCell(cell_coords)) {
			GridOfList<T>& cell = cells[getIndexFromCoords(cell_coords)];
			cell.data.emplace_back(obj);
			return &cell.data.back();
		}
		return nullptr;
	}

	std::list<T*> getAllAt(const sf::Vector2f& position)
	{
		std::list<T*> result;
		const sf::Vector2i cell_coords = getCellCoords(position);

		for (int32_t x(-1); x < 2; ++x) {
			for (int32_t y(-1); y < 2; ++y) {
				const sf::Vector2i coords = cell_coords + sf::Vector2i(x, y);
				if (checkCell(coords)) {
					const uint64_t index = getIndexFromCoords(coords);
					for (GridListCell<T>& m : cells[index]) {
						result.push_back(&m);
					}
				}
			}
		}

		return result;
	}

	GridListCell<T>* getAt(const sf::Vector2f& position)
	{
		const sf::Vector2i cell_coords = getCellCoords(position);

		if (checkCell(cell_coords)) {
			return &cells[getIndexFromCoords(cell_coords)];
		}

		return nullptr;
	}

	bool isEmpty(const sf::Vector2f& position) const
	{
		const sf::Vector2i cell_coords = getCellCoords(position);

		if (checkCell(cell_coords)) {
			return cells[getIndexFromCoords(cell_coords)].empty();
		}

		return true;
	}
};
