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

    RNGf::initialize();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
    // This is because of some crashes on MacOS
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Default : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);
    // Initialize simulation
    Simulation simulation(window);
    // Here you can load your map, please note that
    //  - map size should be 1/4th of 1920x1080 (480x270)
    //  - pixels that have red value > 100 are walls
    //  - pixels that have green value > 100 are food
    MapLoader::loadMap(simulation.world, "res/map.png");
    simulation.distance_field_builder.requestUpdate();
    // Create editor scene around it
    GUI::Scene::Ptr scene = create<edtr::EditorScene>(window, simulation);
    scene->resize();
    // Main loop
	while (window.isOpen()) {
        // Update
        scene->update();
        // Render
        window.clear(sf::Color(100, 100, 100));
        scene->render();
        window.display();
	}
	return 0;
}
