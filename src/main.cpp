#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"


int main()
{
    // Load configuration
    if (Conf::loadUserConf()) {
        std::cout << "Configuration file loaded." << std::endl;
    } else {
        std::cout << "Configuration file couldn't be found." << std::endl;
    }
	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
    window_style = sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);
    // Initialize simulation
    Simulation simulation(window);
    simulation.loadMap("res/map.png");
    // Create editor scene around it
    GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
    // Main loop
	while (window.isOpen()) {
        // Update
        scene->update();
        // Render
        window.clear();
        scene->render();
        window.display();
	}

	return 0;
}
