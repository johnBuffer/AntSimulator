#include <SFML/Graphics.hpp>
#include "simulation/simulation.hpp"


int32_t main()
{
	Conf::loadTextures();
    if (!Conf::loadUserConf()) {
        std::cout << "Couldn't find 'conf.txt', loading default" << std::endl;
    }

	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(2000);

	sim::Simulation simulation;
	const float margin = 160.0f;
	simulation.createColony(margin, margin);
	simulation.createColony(Conf::WORLD_WIDTH - margin, Conf::WORLD_HEIGHT - margin);
	simulation.loadMap("res/map.png");
	
	sf::Clock clock;
    sf::Clock global_clock;
	RMean<float> frame_time(200);
	sf::Text fps_text;
	sf::Font fps_font;
	fps_font.loadFromFile("res/font.ttf");
	fps_text.setFont(fps_font);
	fps_text.setCharacterSize(32);
	fps_text.setFillColor(sf::Color::White);
	fps_text.setPosition(10.0f, 10.0f);
	const float dt = 0.016f;
    float time = 0.0f;
    uint32_t last_colony = 0;
    clock.restart();
	while (true) {
		// Update simulation
		simulation.update();
        frame_time.addValue(clock.restart().asSeconds());
		// Print infos
        if (int(time) % 10 == 0) {
            uint32_t alive = 0;
            int32_t i = 0;
            std::cout << "Time: " << int(time) << "s (speed up: x" << int(dt / frame_time.get()) << ")" << std::endl;
            for (const Colony& c : simulation.colonies) {
                std::cout << "Colony " << i << " pop: " << c.ants.size() << std::endl;
                if (c.ants.size()) {
                    ++alive;
                    last_colony = i;
                }
                ++i;
            }
            if (alive == 1) {
                break;
            }
        }
        time += dt;
	}
    std::cout << "Simulation over (" << global_clock.getElapsedTime().asSeconds() << "s), winner: " << last_colony << std::endl;
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
