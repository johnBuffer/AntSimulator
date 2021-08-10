#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"


int main()
{
	Conf::loadTextures();
    if (!Conf::loadUserConf()) {
        std::cout << "Couldn't find 'conf.txt', loading default" << std::endl;
    }

	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);

	Simulation simulation(window);
	const float margin = 160.0f;
	simulation.createColony(margin, margin);
	simulation.createColony(Conf::WORLD_WIDTH - margin, Conf::WORLD_HEIGHT - margin);
	simulation.loadMap("res/map.png");
	simulation.renderer.vp_handler.reset();
	
	sf::Clock clock;
	RMean<float> fps(100);
	sf::Text fps_text;
	sf::Font fps_font;
	fps_font.loadFromFile("res/font.ttf");
	fps_text.setFont(fps_font);
	fps_text.setCharacterSize(32);
	fps_text.setFillColor(sf::Color::White);
	fps_text.setPosition(10.0f, 10.0f);
	const float dt = 0.016f;

	while (window.isOpen()) {
		// Update simulation
		simulation.processEvents();
		simulation.update(dt);
		// Update FPS metric
		fps_text.setString(toStr(fps.get()));
		// Render simulation
		window.clear(sf::Color(94, 87, 87));
		simulation.render(window);
		window.draw(fps_text);
		window.display();
		// Add render time to the counter
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
