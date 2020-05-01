#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "colony.hpp"
#include "config.hpp"
#include "display_manager.hpp"


int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(Conf<>::WIN_WIDTH, Conf<>::WIN_HEIGHT), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);

	srand(11);

	World world(Conf<>::WIN_WIDTH, Conf<>::WIN_HEIGHT);
	Colony colony(800, 800, 512);

	world.grid_markers_home.add(Marker(colony.position, Marker::ToHome, 100000.0f, true));
	
	DisplayManager display_manager(window, window, world, colony);

	sf::Vector2f last_clic;

	while (window.isOpen())
	{
		display_manager.processEvents();

		if (display_manager.clic) {
			const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			const sf::Vector2f world_position = display_manager.displayCoordToWorldCoord(sf::Vector2f(mouse_position.x, mouse_position.y));
			const float clic_min_dist = 2.0f;
			if (getLength(world_position - last_clic) > clic_min_dist) {
				world.addFoodAt(world_position.x, world_position.y, 5.0f);
				last_clic = world_position;
			}
		}

		const float dt = 0.016f;

		colony.update(dt, world);
		world.update(dt);

		window.clear(sf::Color::Black);
		
		display_manager.draw();

		window.display();
	}

	return 0;
}