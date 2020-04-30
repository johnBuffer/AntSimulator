#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "colony.hpp"
#include "config.hpp"


int main()
{
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode(Conf<>::WIN_WIDTH, Conf<>::WIN_HEIGHT), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(144);

	srand(11);

	World world(Conf<>::WIN_WIDTH, Conf<>::WIN_HEIGHT);

	Colony colony(800, 800, 512);
	world.grid_markers_home.add(Marker(colony.position, Marker::ToHome, 100000.0f, true));

	bool clicking = false;
	sf::Vector2i last_clic;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) {
				window.close();
			}
			else if (event.type == sf::Event::MouseButtonPressed) {
				clicking = true;
				last_clic = sf::Mouse::getPosition(window);
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				clicking = false;
			}
		}

		if (clicking) {
			const sf::Vector2i mouse_position = sf::Mouse::getPosition(window);
			const float clic_min_dist = 2.0f;
			if (getLength(mouse_position - last_clic) > clic_min_dist) {
				world.addFoodAt(mouse_position.x, mouse_position.y, 5.0f);
				last_clic = mouse_position;
			}
		}

		const float dt = 0.016f;

		colony.update(dt, world);
		world.update(dt);

		window.clear();
		
		world.render(window);
		colony.render(window);

		window.display();
	}

	return 0;
}