#include <SFML/Graphics.hpp>
#include <list>
#include <fstream>
#include "simulation/config.hpp"
#include "simulation/world/distance_field_builder.hpp"
#include "simulation/simulation.hpp"
#include "editor/editor_scene.hpp"
#include "editor/game_scene.hpp"


int main()
{
    if (Conf::loadUserConf()) {
        std::cout << "OK" << std::endl;
    } else {
        std::cout << "Not OK" << std::endl;
    }
	sf::ContextSettings settings;
	settings.antialiasingLevel = 1;
    int32_t window_style = Conf::USE_FULLSCREEN ? sf::Style::Fullscreen : sf::Style::Default;
	sf::RenderWindow window(sf::VideoMode(Conf::WIN_WIDTH, Conf::WIN_HEIGHT), "AntSim", window_style, settings);
	window.setFramerateLimit(60);

    // Main scenes
    GUI::Scene::Ptr editor = create<edtr::EditorScene>(window);
    GUI::Scene::Ptr game = edtr::createGameScene(window);
    GUI::Scene::Ptr current_scene = editor;
    // Add switches
    const sf::Vector2f switch_size(120.0f, 40.0f);
    SPtr<GUI::Button> editor_to_game = create<GUI::Button>("To Game View", [&](){
        current_scene = game;
    });
    editor_to_game->setSize(switch_size);
    editor->addItem(editor_to_game, "ToGame", GUI::Alignement::Right);
    SPtr<GUI::Button> game_to_editor = create<GUI::Button>("To Editor View", [&](){
        current_scene = editor;
    });
    game_to_editor->setSize(switch_size);
    game->getByName<edtr::GameView>("view")->addItem(game_to_editor, "", GUI::Alignement::Right);

	while (window.isOpen()) {
		current_scene->update();
        // Render
        window.clear();
        current_scene->render();
        window.display();
	}

    current_scene = nullptr;
    editor = nullptr;

	return 0;
}

#if defined(_WIN32)
#include <windows.h>
int APIENTRY WinMain(HINSTANCE, HINSTANCE, PSTR, int)
{
  return main();
}
#endif
