#pragma once

#include <memory>
#include <SFML/Graphics.hpp>

#include "common/configuration_loader.hpp"
#include "common/configuration_helper.hpp"


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
	static float MARKER_INTENSITY;
	static sf::Vector2f COLONY_POSITION;
    static sf::Vector2u WINDOW_SIZE;
	static uint32_t WORLD_WIDTH;
	static uint32_t WORLD_HEIGHT;
	static uint32_t ANT_COUNT;
	static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	static std::shared_ptr<sf::Texture> MARKER_TEXTURE;
    static constexpr uint32_t MAX_COLONIES_COUNT = 4;
	static sf::Color COLONY_COLORS[MAX_COLONIES_COUNT];
    static uint32_t USE_FULLSCREEN;
    static float GUI_SCALE;

	static void loadTextures()
	{
		DefaultConf::ANT_TEXTURE = std::make_shared<sf::Texture>();
		DefaultConf::ANT_TEXTURE->loadFromFile("res/ant.png");
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
    
    static bool loadUserConf(std::string const& conf_filepath)
    {
	    cload::ConfigurationLoader const loader{conf_filepath};
        if (loader.isValid()) {
            loadInto(loader, "window_size", &DefaultConf<T>::WINDOW_SIZE);
            loadInto(loader, "fullscreen", &DefaultConf<T>::USE_FULLSCREEN);
            loadInto(loader, "ant_count", &DefaultConf<T>::ANT_COUNT);
            return true;
        }
	    return false;
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
const sf::Color DefaultConf<T>::WALL_COLOR = sf::Color(114, 107, 107);
template<typename T>
sf::Vector2u DefaultConf<T>::WINDOW_SIZE = {1920, 1080};
template<typename T>
uint32_t DefaultConf<T>::WORLD_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WORLD_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::ANT_COUNT = 3000;
template<typename T>
float DefaultConf<T>::COLONY_SIZE = 20.0f;
template<typename T>
float DefaultConf<T>::MARKER_INTENSITY = 8000.0f;
template<typename T>
sf::Vector2f DefaultConf<T>::COLONY_POSITION = sf::Vector2f(500.0f, DefaultConf<T>::WINDOW_SIZE.y * 0.5f);

template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::ANT_TEXTURE;
template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::MARKER_TEXTURE;
template<typename T>
sf::Color DefaultConf<T>::COLONY_COLORS[MAX_COLONIES_COUNT] = {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color(50, 255, 255)};

template<typename T>
uint32_t DefaultConf<T>::USE_FULLSCREEN = 1;

template<typename T>
float DefaultConf<T>::GUI_SCALE = 1.0f;

using Conf = DefaultConf<int>;



