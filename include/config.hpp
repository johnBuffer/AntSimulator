#pragma once

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
	inline const static uint8_t THREAD_COUNT = 1;
	
	inline const static uint32_t WIN_WIDTH = 1600;
	inline const static uint32_t WIN_HEIGHT = 900;

	inline static sf::Texture* ANT_TEXTURE;
};

