#pragma once

#include <memory>
#include <SFML/Graphics.hpp>


template<typename T = void>
struct DefaultConf
{
	const static sf::Color ANT_COLOR;
	const static sf::Color FOOD_COLOR;
	const static sf::Color TO_FOOD_COLOR;
	const static sf::Color TO_HOME_COLOR;
	const static sf::Color COLONY_COLOR;
	static float COLONY_SIZE;
	static sf::Vector2f COLONY_POSITION;
	static uint32_t WIN_WIDTH;
	static uint32_t WIN_HEIGHT;
	static uint32_t ANTS_COUNT;
	static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	static std::shared_ptr<sf::Texture> MARKER_TEXTURE;

	static void loadTextures()
	{
		DefaultConf::ANT_TEXTURE = std::make_shared<sf::Texture>();
		DefaultConf::ANT_TEXTURE->loadFromFile("res/ant_2.png");
		DefaultConf::ANT_TEXTURE->setSmooth(true);
		DefaultConf::MARKER_TEXTURE = std::make_shared<sf::Texture>();
		DefaultConf::MARKER_TEXTURE->loadFromFile("res/marker.png");
		DefaultConf::MARKER_TEXTURE->setSmooth(true);
	}

	static void freeTextures()
	{
		DefaultConf::ANT_TEXTURE = nullptr;
		DefaultConf::MARKER_TEXTURE = nullptr;
	}
};


using Conf = DefaultConf<>;

const sf::Color Conf::ANT_COLOR = sf::Color(255, 73, 68);
const sf::Color Conf::FOOD_COLOR = sf::Color(66, 153, 66);
const sf::Color Conf::TO_FOOD_COLOR = sf::Color(0, 255, 0);
const sf::Color Conf::TO_HOME_COLOR = sf::Color(255, 0, 0);
const sf::Color Conf::COLONY_COLOR = Conf::ANT_COLOR;
uint32_t Conf::WIN_WIDTH = 1920;
uint32_t Conf::WIN_HEIGHT = 1080;
uint32_t Conf::ANTS_COUNT = 1024;
float Conf::COLONY_SIZE = 20.0f;
sf::Vector2f Conf::COLONY_POSITION = sf::Vector2f(Conf::WIN_WIDTH * 0.5f, Conf::WIN_HEIGHT * 0.5f);
//sf::Vector2f Conf::COLONY_POSITION = sf::Vector2f(100.0f, 100.0f);

std::shared_ptr<sf::Texture> Conf::ANT_TEXTURE;
std::shared_ptr<sf::Texture> Conf::MARKER_TEXTURE;

