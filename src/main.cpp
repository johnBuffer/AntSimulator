#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "colony.hpp"


int main()
{
	const uint32_t win_width(1920);
	const uint32_t win_height(1080);
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(144);

	srand(11);

	World world(win_width, win_height);

	Colony colony(600, 650, 500);
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
				world.addFoodAt(mouse_position.x, mouse_position.y, 10.0f);
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