#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"


int main(int32_t argc, char** argv)
{
    if (argc > 0) {
        // Should be always true but we never know
        ResourceStore::initialize(argv[0]);
        ResourceStore::loadFont("res/font.ttf", "font");
        ResourceStore::loadTexture("res/ant.png", "ant");
        ResourceStore::loadTexture("res/marker.png", "marker");
    }

    // Load configuration
    if (Conf::loadUserConf()) {
        std::cout << "Configuration file loaded." << std::endl;
    } else {
        std::cout << "Configuration file couldn't be found." << std::endl;
    }

    RNGf::initialize();

	sf::ContextSettings settings;
	settings.antialiasingLevel = 4;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Default : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);
    // Initialize simulation
    Simulation simulation(window);
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
