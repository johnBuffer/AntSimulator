#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "sound_player.hpp"


template<typename T = int>
struct Conf
{
	inline const static sf::Color ANT_COLOR = sf::Color(255, 73, 68);
	inline const static sf::Color FOOD_COLOR = sf::Color(29, 129, 18);
	inline const static sf::Color TO_FOOD_COLOR = sf::Color(119, 211, 109);
	inline const static sf::Color TO_HOME_COLOR = sf::Color(122, 105, 199);
	inline const static sf::Color COLONY_COLOR = sf::Color(67, 46, 163);
	inline const static uint32_t MAX_MARKERS_PER_CELL = 1024;
	
	inline const static uint32_t WIN_WIDTH = 1600;
	inline const static uint32_t WIN_HEIGHT = 900;

	inline static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	inline static std::shared_ptr<sf::Texture> MARKER_TEXTURE;

	static void loadTextures()
	{
		Conf<>::ANT_TEXTURE = std::make_shared<sf::Texture>();
		Conf<>::ANT_TEXTURE->loadFromFile("res/ant_2.png");
		Conf<>::ANT_TEXTURE->setSmooth(true);

		Conf<>::MARKER_TEXTURE = std::make_shared<sf::Texture>();
		Conf<>::MARKER_TEXTURE->loadFromFile("res/circle.png");
		Conf<>::MARKER_TEXTURE->setSmooth(true);
	}

	static void freeTextures()
	{
		Conf<>::ANT_TEXTURE = nullptr;
		Conf<>::MARKER_TEXTURE = nullptr;
	}
};


