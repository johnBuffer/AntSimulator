#pragma once
#include "world.hpp"
#include "distance_field_builder.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


struct MapLoader
{
	static void loadMap(World& world, const std::string& map_filename)
	{
        constexpr uint8_t wall_0_value = 100;
        constexpr uint8_t food_0_value = 100;
		sf::Image food_map;
		if (food_map.loadFromFile(map_filename)) {
			for (uint32_t x(0); x < food_map.getSize().x; ++x) {
				for (uint32_t y(0); y < food_map.getSize().y; ++y) {
					const sf::Vector2f position = float(world.map.cell_size) * sf::Vector2f(to<float>(x), to<float>(y));
					if (food_map.getPixel(x, y).r > wall_0_value) {
						world.addWall(position);
					}
					else if (food_map.getPixel(x, y).g > food_0_value) {
						const uint32_t green = food_map.getPixel(x, y).g - food_0_value;
						world.addFoodAt(position.x, position.y, green);
					}
				}
			}
		}
	}
};

