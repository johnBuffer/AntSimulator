#pragma once
#include "world.hpp"
#include "distance_field_builder.hpp"
#include <SFML/Graphics.hpp>


struct MapLoader
{
	static void load(World& world, const std::string& map_filename)
	{
		sf::Image food_map;
		if (food_map.loadFromFile(map_filename)) {
			for (uint32_t x(0); x < food_map.getSize().x; ++x) {
				for (uint32_t y(0); y < food_map.getSize().y; ++y) {
					const sf::Vector2f position = float(world.map.cell_size) * sf::Vector2f(to<float>(x), to<float>(y));
					if (food_map.getPixel(x, y).r > 100) {
						world.addWall(position);
					}
					else if (food_map.getPixel(x, y).g > 100) {
						const float green = food_map.getPixel(x, y).g;
						world.addFoodAt(position.x, position.y, to<int32_t>(green * 0.025f));
					}
				}
			}
		}

		DistanceFieldBuilder::computeDistance(world.map);
	}
};

