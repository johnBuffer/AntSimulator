#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include <fstream>
#include "colony.hpp"
#include "config.hpp"
#include "display_manager.hpp"


uint32_t loadUserConf()
{
	uint32_t ants_count = 512;
	std::ifstream conf_file("conf.txt");
	if (conf_file) {
		conf_file >> ants_count;
	}
	else {
		std::cout << "Couldn't find 'conf.txt', loading default" << std::endl;
	}

	return ants_count;
}


int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", sf::Style::Default, settings);
	window.setFramerateLimit(60);

	Conf::loadTextures();
	const uint32_t ants_count = loadUserConf();

	World world(Conf::WIN_WIDTH, Conf::WIN_HEIGHT);
	Colony colony(Conf::WIN_WIDTH/2, Conf::WIN_HEIGHT/2, ants_count);
	world.addMarker(Marker(colony.position, Marker::ToHome, 10.0f, true));
	
	DisplayManager display_manager(window, window, world, colony);

	sf::Vector2f last_clic;

	while (window.isOpen())
	{
		display_manager.processEvents();

		// Add food on clic
		if (display_manager.clic) {
			const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			const sf::Vector2f world_position = display_manager.displayCoordToWorldCoord(sf::Vector2f(to<float>(mouse_position.x), to<float>(mouse_position.y)));
			const float clic_min_dist = 2.0f;
			if (getLength(world_position - last_clic) > clic_min_dist) {
				world.addFoodAt(world_position.x, world_position.y, 5.0f);
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