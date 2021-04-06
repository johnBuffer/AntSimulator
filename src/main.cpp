#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <fstream>
#include "colony.hpp"
#include "config.hpp"
#include "display_manager.hpp"



void loadUserConf()
{
	std::ifstream conf_file("conf.txt");
	if (conf_file) {
		conf_file >> Conf::WIN_WIDTH;
		conf_file >> Conf::WIN_HEIGHT;
		conf_file >> Conf::ANTS_COUNT;
	}
	else {
		std::cout << "Couldn't find 'conf.txt', loading default" << std::endl;
	}
}


int main()
{
	Conf::loadTextures();
	loadUserConf();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);

	World world(Conf::WIN_WIDTH, Conf::WIN_HEIGHT);
	Colony colony(Conf::COLONY_POSITION.x, Conf::COLONY_POSITION.y, Conf::ANTS_COUNT);
	//Colony colony(250, 250, 512);
	for (uint32_t i(0); i < 64; ++i) {
		float angle = float(i) / 64.0f * (2.0f * PI);
		world.addMarker(Marker(colony.position + 16.0f * sf::Vector2f(cos(angle), sin(angle)), Marker::ToHome, 10.0f, true));
	}
	
	DisplayManager display_manager(window, window, world, colony);

	sf::Vector2f last_clic;

	sf::Image wall_map;
	if (wall_map.loadFromFile("map.bmp")) {
		for (uint32_t x(0); x < wall_map.getSize().x; ++x) {
			for (uint32_t y(0); y < wall_map.getSize().y; ++y) {
				const sf::Vector2f position = float(world.grid_walls.cell_size) * sf::Vector2f(to<float>(x), to<float>(y));
				if (wall_map.getPixel(x, y).r > 50) {
					world.addWall(position);
				}
				else if (wall_map.getPixel(x, y).g > 100) {
					world.addFoodAt(position.x, position.y, 5.0f);
				}
			}
		}
	}

	while (window.isOpen())
	{
		display_manager.processEvents();

		// Add food on clic
		if (display_manager.clic) {
			const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			const sf::Vector2f world_position = display_manager.displayCoordToWorldCoord(sf::Vector2f(to<float>(mouse_position.x), to<float>(mouse_position.y)));
			const float clic_min_dist = 2.0f;
			if (getLength(world_position - last_clic) > clic_min_dist) {
				if (display_manager.wall_mode) {
					world.addWall(world_position);
				}
				else if (display_manager.remove_wall) {
					world.removeWall(world_position);
				}
				else {
					world.addFoodAt(world_position.x, world_position.y, 10.0f);
				}
				last_clic = world_position;
			}
		}

		const float dt = 0.016f;

		if (!display_manager.pause) {
			colony.update(dt, world);
			world.update(dt);
		}

		window.clear(sf::Color(94, 87, 87));
		
		display_manager.draw();

		window.display();
	}

	// Free textures
	Conf::freeTextures();

	return 0;
}

#if defined(_WIN32)
#include <windows.h>
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline,
                     int cmdshow) {
  return main();
}
#endif