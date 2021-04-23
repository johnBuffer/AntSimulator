#pragma once

#include <memory>
#include <SFML/Graphics.hpp>


template<typename T>
struct DefaultConf
{
	const static sf::Color ANT_COLOR;
	const static sf::Color FOOD_COLOR;
	const static sf::Color TO_FOOD_COLOR;
	const static sf::Color TO_HOME_COLOR;
	const static sf::Color COLONY_COLOR;
	const static sf::Color WALL_COLOR;
	static float COLONY_SIZE;
	static sf::Vector2f COLONY_POSITION;
	static uint32_t WIN_WIDTH;
	static uint32_t WIN_HEIGHT;
	static uint32_t WORLD_WIDTH;
	static uint32_t WORLD_HEIGHT;
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

template<typename T>
const sf::Color DefaultConf<T>::ANT_COLOR = sf::Color(255, 73, 68);
template<typename T>
const sf::Color DefaultConf<T>::FOOD_COLOR = sf::Color(66, 153, 66);
template<typename T>
const sf::Color DefaultConf<T>::TO_FOOD_COLOR = sf::Color(0, 255, 0);
template<typename T>
const sf::Color DefaultConf<T>::TO_HOME_COLOR = sf::Color(255, 0, 0);
template<typename T>
const sf::Color DefaultConf<T>::COLONY_COLOR = DefaultConf<T>::ANT_COLOR;
template<typename T>
const sf::Color DefaultConf<T>::WALL_COLOR = sf::Color(94, 87, 87);
template<typename T>
uint32_t DefaultConf<T>::WIN_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WIN_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::WORLD_WIDTH = 2000;
template<typename T>
uint32_t DefaultConf<T>::WORLD_HEIGHT = 1000;
template<typename T>
uint32_t DefaultConf<T>::ANTS_COUNT = 1024;
template<typename T>
float DefaultConf<T>::COLONY_SIZE = 20.0f;
template<typename T>
sf::Vector2f DefaultConf<T>::COLONY_POSITION = sf::Vector2f(DefaultConf<T>::WIN_WIDTH * 0.5f, DefaultConf<T>::WIN_HEIGHT * 0.5f);

template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::ANT_TEXTURE;
template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::MARKER_TEXTURE;

using Conf = DefaultConf<int>;
