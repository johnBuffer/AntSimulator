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
#include "editor/world_view.hpp"
#include "time_control/time_controller.hpp"


namespace edtr
{

struct EditorScene : public GUI::Scene
{
    using Ptr = std::shared_ptr<EditorScene>;

    Simulation&  simulation;
    ControlState control_state;

    SPtr<Toolbox>      toolbox;
    SPtr<WorldView>    renderer;
    SPtr<ToolSelector> tool_selector;

    explicit
    EditorScene(sf::RenderWindow& window, Simulation& sim)
        : GUI::Scene(window)
        , simulation(sim)
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
        const sf::Vector2u window_size = window.getSize();

        renderer = create<WorldView>(toVector2f(window_size), simulation, control_state);

        toolbox = create<Toolbox>(sf::Vector2f(350.0f, to<float>(window_size.y)));
        // Add map edition tools
        tool_selector = create<ToolSelector>();
        toolbox->addItem(tool_selector);
        // Add colonies edition tools
        auto colonies = create<ColonyCreator>(simulation, control_state);
        toolbox->addItem(colonies);
        // Add time controls
        auto time_controls = create<TimeController>();
        renderer->watch(time_controls, [this, time_controls](){
            this->renderer->current_time_state = time_controls->current_state;
            if (time_controls->current_state == TimeController::State::Speed) {
                this->window.setFramerateLimit(0);
            } else {
                this->window.setFramerateLimit(60);
            }
        });

        addItem(renderer);
        addItem(toolbox, "Toolbox");
        addItem(time_controls, "", GUI::Alignement::Right);
    }
};

}
