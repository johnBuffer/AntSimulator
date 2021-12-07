#pragma once
#include "world_grid.hpp"


struct DistanceFieldBuilder
{
	static void computeDistance(WorldGrid& grid)
	{
		for (int32_t x(0); x < grid.width; ++x) {
			for (int32_t y(0); y < grid.height; ++y) {
				WorldCell& cell = grid.get(sf::Vector2i(x, y));
				if (cell.wall) {
					cell.wall_dist = getMinDist(x, y, grid, false, 20);
				}
				else {
					cell.wall_dist = getMinDist(x, y, grid, true, 3);
				}
			}
		}
	}

	static float getMinDist(int32_t x, int32_t y, WorldGrid& grid, bool dist_to_wall, int32_t max_iteration)
	{
		auto min_dist = to<float>(max_iteration);
		for (int32_t dx(-max_iteration); dx <= max_iteration; ++dx) {
			for (int32_t dy(-max_iteration); dy <= max_iteration; ++dy) {
				const WorldCell* cell = grid.getSafe(sf::Vector2i(x + dx, y + dy));
				if (cell) {
					if ((cell->wall && dist_to_wall) || (!cell->wall && !dist_to_wall)) {
						const float dist = getLength(sf::Vector2f(to<float>(dx), to<float>(dy)));
						if (dist < min_dist) {
							min_dist = dist;
						}
					}
				}
			}
		}
		min_dist = std::min(1.0f, min_dist / float(max_iteration));
		return min_dist;
	}
};
