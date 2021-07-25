#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "config.hpp"
#include "distance_field_builder.hpp"
#include "simulation.hpp"


int main()
{
	Conf::loadTextures();
	MapLoader::loadUserConf();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);

	Simulation simulation(window);
	simulation.createColony(78.0f*4.0f, 160.0f);
	simulation.createColony(451.0f * 4.0f, 77.0f * 4.0f);
    simulation.createColony(116.0f * 4.0f, 260.0f * 4.0f);
	simulation.loadMap("res/map.png");
	
	sf::Clock clock;
	RMean<float> fps(100);
	sf::Text fps_text;
	sf::Font fps_font;
	fps_font.loadFromFile("res/font.ttf");
	fps_text.setFont(fps_font);
	fps_text.setCharacterSize(32);
	fps_text.setFillColor(sf::Color::White);
	fps_text.setPosition(10.0f, 10.0f);

	while (window.isOpen())
	{
		simulation.processEvents();

		const float dt = 0.016f;
		simulation.update(dt);

		fps_text.setString(toStr(fps.get()));

		window.clear(sf::Color(94, 87, 87));
		simulation.render(window);
		window.draw(fps_text);
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
