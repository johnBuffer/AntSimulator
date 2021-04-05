#pragma once

#include <memory>
#include <SFML/Graphics.hpp>


template<typename T = int>
struct DefaultConf
{
	const static sf::Color ANT_COLOR;
	const static sf::Color FOOD_COLOR;
	const static sf::Color TO_FOOD_COLOR;
	const static sf::Color TO_HOME_COLOR;
	const static sf::Color COLONY_COLOR;
	const static uint32_t MAX_MARKERS_PER_CELL = 1024;
	
	const static uint32_t WIN_WIDTH = 1920;
	const static uint32_t WIN_HEIGHT = 1080;

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

using Conf = DefaultConf<>;

const sf::Color Conf::ANT_COLOR = sf::Color(255, 73, 68);
const sf::Color Conf::FOOD_COLOR = sf::Color(66, 153, 66);
const sf::Color Conf::TO_FOOD_COLOR = sf::Color(0, 255, 0);
const sf::Color Conf::TO_HOME_COLOR = sf::Color(255, 0, 0);
const sf::Color Conf::COLONY_COLOR = Conf::ANT_COLOR;

std::shared_ptr<sf::Texture> Conf::ANT_TEXTURE;
std::shared_ptr<sf::Texture> Conf::MARKER_TEXTURE;

