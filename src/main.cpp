#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "config.hpp"
#include "display_manager.hpp"
#include "distance_field_builder.hpp"
#include "simulation.hpp"


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
	settings.antialiasingLevel = 4;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);

	Simulation simulation(window);
	simulation.createColony(100.0f, 100.0f);
	simulation.createColony(100.0f, Conf::WIN_HEIGHT - 100.0f);
	simulation.loadMap("res/map.bmp");
	
	sf::Vector2f last_clic;

	sf::Clock clock;
	RMean<float> fps(100);

	while (window.isOpen())
	{
		simulation.processEvents();

		const float dt = 0.016f;
		simulation.update(dt);

		window.clear(sf::Color(94, 87, 87));
		simulation.render(window);
		window.display();

		fps.addValue(1.0f / clock.restart().asSeconds());
	}

	// Free textures
	Conf::freeTextures();

	return 0;
}

#if defined(_WIN32)
#include <windows.h>
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
  return main();
}
#endif