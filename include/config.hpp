#pragma once

#include <SFML/Graphics.hpp>


template<typename T = int>
struct Conf
{
	inline const static sf::Color ANT_COLOR = sf::Color(255, 73, 68);
	inline const static sf::Color FOOD_COLOR = sf::Color(255, 205, 31);
	inline const static sf::Color TO_FOOD_COLOR = sf::Color(255, 228, 117);
	inline const static sf::Color TO_HOME_COLOR = sf::Color(122, 105, 199);
	inline const static sf::Color COLONY_COLOR = sf::Color(67, 46, 163);
};

