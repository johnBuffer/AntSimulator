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
	//world.grid.addMarker(Marker(sf::Vector2f(500.0f, 500.0f), Marker::ToFood, 0.0f, 100.0f));
	world.food = Food(300, 300, 10);
	world.grid.addMarker(Marker(world.food.position, Marker::ToFood, 0.0f, 1000.0f, true));

	Colony colony(600, 650, 40);
	world.grid.addMarker(Marker(colony.position, Marker::ToHome, 0.0f, 1000.0f, true));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
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