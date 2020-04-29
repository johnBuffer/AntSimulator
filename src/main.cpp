#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "colony.hpp"


int main()
{
	sf::RenderWindow window(sf::VideoMode(1600, 900), "AntSim");
	//window.setFramerateLimit(144);

	srand(10);

	Colony colony(1200, 750, 100);
	Food food(200, 200, 100);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		colony.update(0.016f, food);

		window.clear();
		
		colony.render(window);
		food.render(window);

		window.display();
	}

	return 0;
}