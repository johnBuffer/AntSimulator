#pragma once
#include "world.hpp"
#include "distance_field_builder.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>


struct MapLoader
{
	static void loadUserConf()
	{
		std::ifstream conf_file("conf.txt");
		if (conf_file) {
			conf_file >> Conf::WIN_WIDTH;
			conf_file >> Conf::WIN_HEIGHT;
			conf_file >> Conf::ANTS_COUNT;
		}
		else {
			std::cout << "Couldn't find 'conf.txt', loading default" << std::endl;
		}
	}

	static void loadMap(World& world, const std::string& map_filename)
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
						world.addFoodAt(position.x, position.y, 20);
					}
				}
			}
		}
		DistanceFieldBuilder::computeDistance(world.map);
	}
};

