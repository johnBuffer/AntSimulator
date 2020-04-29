#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "colony.hpp"


int main()
{
	const uint32_t win_width(1600);
	const uint32_t win_height(900);
	sf::RenderWindow window(sf::VideoMode(win_width, win_height), "AntSim");
	//window.setFramerateLimit(144);

	srand(10);

	World world(win_width, win_height);
	world.food = Food(200, 200, 100);

	Colony colony(1200, 750, 500);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		const float dt = 0.016f;
		world.update(dt);
		colony.update(dt, world);

		window.clear();
		
		colony.render(window);
		world.render(window);

		window.display();
	}

	return 0;
}