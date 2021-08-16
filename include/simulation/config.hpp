#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include <fstream>


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
	static uint32_t WIN_WIDTH;
	static uint32_t WIN_HEIGHT;
	static uint32_t WORLD_WIDTH;
	static uint32_t WORLD_HEIGHT;
	static uint32_t ANTS_COUNT;
	static std::shared_ptr<sf::Texture> ANT_TEXTURE;
	static std::shared_ptr<sf::Texture> MARKER_TEXTURE;
	static sf::Color COLONY_COLORS[8];
    static uint32_t USE_FULLSCREEN;

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
    
    static bool loadUserConf()
    {
        std::ifstream conf_file("conf.txt");
        if (conf_file) {
            std::string line;
            uint32_t line_count = 0;
            while (std::getline(conf_file, line)) {
                if (line[0] == '#') {
                    continue;
                }
                const char* line_c = line.c_str();
                switch (line_count) {
                    case 0:
                        DefaultConf<T>::WIN_WIDTH = std::atoi(line_c);
                        break;
                    case 1:
                        DefaultConf<T>::WIN_HEIGHT = std::atoi(line_c);
                        break;
                    case 2:
                        DefaultConf<T>::USE_FULLSCREEN = std::atoi(line_c);
                        break;
                    case 3:
                        DefaultConf<T>::ANTS_COUNT = std::atoi(line_c);
                        break;
                    default:
                        break;
                }
                ++line_count;
            }
        } else {
            return false;
        }
        return true;
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
uint32_t DefaultConf<T>::WIN_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WIN_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::WORLD_WIDTH = 1920;
template<typename T>
uint32_t DefaultConf<T>::WORLD_HEIGHT = 1080;
template<typename T>
uint32_t DefaultConf<T>::ANTS_COUNT = 3000;
template<typename T>
float DefaultConf<T>::COLONY_SIZE = 20.0f;
template<typename T>
float DefaultConf<T>::MARKER_INTENSITY = 8000.0f;
template<typename T>
sf::Vector2f DefaultConf<T>::COLONY_POSITION = sf::Vector2f(500.0f, DefaultConf<T>::WIN_HEIGHT * 0.5f);

template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::ANT_TEXTURE;
template<typename T>
std::shared_ptr<sf::Texture> DefaultConf<T>::MARKER_TEXTURE;
template<typename T>
sf::Color DefaultConf<T>::COLONY_COLORS[8] = {sf::Color::Red, sf::Color::Blue, sf::Color::Yellow, sf::Color(50, 255, 255)};

template<typename T>
uint32_t DefaultConf<T>::USE_FULLSCREEN = 1;

using Conf = DefaultConf<int>;



