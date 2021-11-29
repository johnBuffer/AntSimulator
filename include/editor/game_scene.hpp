#pragma once
#include "game_view.hpp"


namespace edtr
{

GUI::Scene::Ptr createGameScene(sf::RenderWindow& window)
{
    GUI::ItemPtr game_view = std::make_shared<GameView>(toVector2f(window.getSize()));
    GUI::Scene::Ptr game_scene = std::make_shared<GUI::Scene>(window);
    game_scene->addItem(game_view, "view");
    
    return game_scene;
}

}
