#pragma once
#include "GUI/scene.hpp"
#include "toolbox.hpp"
#include "editor/color_picker/color_picker.hpp"
#include "color_saver.hpp"
#include "GUI/utils.hpp"
#include "set_color_button.hpp"
#include "GUI/named_container.hpp"
#include "tool_selector.hpp"
#include "editor/colony_creator/colony_creator.hpp"
#include "simulation/world/world.hpp"
#include "render/renderer.hpp"
#include "simulation/config.hpp"
#include "editor/ world_view.hpp"


namespace edtr
{

struct EditorScene : public GUI::Scene
{
    using Ptr = std::shared_ptr<EditorScene>;

    World world;

    SPtr<Toolbox> toolbox;
    SPtr<WorldView> renderer;
    SPtr<ColorPicker> color_picker;
    SPtr<ColorSaver> color_saver;
    SPtr<SetColorButton> set_color_button;
    SPtr<ToolSelector> tool_selector;

    explicit
    EditorScene(sf::RenderWindow& window)
        : GUI::Scene(window)
        , world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
    {
        Conf::loadTextures();
        initialize();
    }

    ~EditorScene()
    {
        std::cout << "Exiting, clean resources" << std::endl;
        Conf::freeTextures();
    }
    
    void initialize()
    {
        MapLoader::loadMap(world, "res/map.png");

        const sf::Vector2u window_size = window.getSize();

        renderer = create<WorldView>(toVector2f(window_size), world);

        toolbox = create<Toolbox>(sf::Vector2f(350.0f, to<float>(window_size.y)));
        tool_selector = create<ToolSelector>();

        toolbox->addItem(tool_selector);

        auto colonies = create<ColonyCreator>();
        toolbox->addItem(colonies);

        toolbox->addItem(create<ColorPicker>());

        addItem(renderer);
        addItem(toolbox, "Toolbox");
    }
};

}
