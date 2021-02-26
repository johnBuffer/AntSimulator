#pragma once

#include <memory>
#include <SFML/Graphics.hpp>


struct Conf
{
	const static sf::Color ANT_COLOR;
	const static sf::Color FOOD_COLOR;
	const static sf::Color TO_FOOD_COLOR;
	const static sf::Color TO_HOME_COLOR;
	const static sf::Color COLONY_COLOR;
	const static uint32_t MAX_MARKERS_PER_CELL;
	const static uint32_t WIN_WIDTH;
	const static uint32_t WIN_HEIGHT;
	static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	static std::shared_ptr<sf::Texture> MARKER_TEXTURE;

	static void loadTextures()
	{
		Conf::ANT_TEXTURE = std::make_shared<sf::Texture>();
		Conf::ANT_TEXTURE->loadFromFile("res/ant_2.png");
		Conf::ANT_TEXTURE->setSmooth(true);

		Conf::MARKER_TEXTURE = std::make_shared<sf::Texture>();
		Conf::MARKER_TEXTURE->loadFromFile("res/marker.png");
		Conf::MARKER_TEXTURE->setSmooth(true);
	}

	static void freeTextures()
	{
		Conf::ANT_TEXTURE = nullptr;
		Conf::MARKER_TEXTURE = nullptr;
	}
};
