#include "config.hpp"

const sf::Color Conf::ANT_COLOR = sf::Color(255, 73, 68);
const sf::Color Conf::FOOD_COLOR = sf::Color(29, 129, 18);
const sf::Color Conf::TO_FOOD_COLOR = sf::Color(119, 211, 109);
const sf::Color Conf::TO_HOME_COLOR = sf::Color(122, 105, 199);
const sf::Color Conf::COLONY_COLOR = sf::Color(67, 46, 163);
const uint32_t Conf::MAX_MARKERS_PER_CELL = 1024;
const uint32_t Conf::WIN_WIDTH = 1920;
const uint32_t Conf::WIN_HEIGHT = 1080;
std::shared_ptr<sf::Texture> Conf::ANT_TEXTURE;
std::shared_ptr<sf::Texture> Conf::MARKER_TEXTURE;
