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
#include "display_options/display_options.hpp"


namespace edtr
{

struct EditorScene : public GUI::Scene
{
    using Ptr = std::shared_ptr<EditorScene>;

    Simulation&  simulation;
    ControlState control_state;

    SPtr<Toolbox>       toolbox;
    SPtr<WorldView>     renderer;
    SPtr<ToolSelector>  tool_selector;
    SPtr<DisplayOption> display_controls;

    explicit
    EditorScene(sf::RenderWindow& window, Simulation& sim)
        : GUI::Scene(window)
        , simulation(sim)
    {
        Conf::loadTextures();
        initialize();

        root.padding = 0.0f;
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
        // Add display options
        display_controls = create<DisplayOption>(control_state);
        watch(display_controls, [this](){
            updateRenderOptions();
        });
        toolbox->addItem(display_controls);

        // Add map edition tools
        auto tools = create<GUI::NamedContainer>("Edit Map", GUI::Container::Orientation::Vertical);
        tools->header->addItem(create<GUI::EmptyItem>());
        tools->hideRoot();
        auto tools_toggle = create<GUI::Toggle>();
        tools_toggle->color_on = {240, 180, 0};
        tools->watch(tools_toggle, [this, tools_toggle, tools](){
            control_state.show_brush_preview = tools_toggle->state;

            if (tools_toggle->state) {
                tools->showRoot();
                this->tool_selector->setEditMode(tools_toggle->state);
            } else {
                tools->hideRoot();
                this->tool_selector->resetCallback();
            }
        });
        //tools_toggle->setState(false);

        tools->header->addItem(tools_toggle);
        toolbox->addItem(tools);

        tool_selector = create<ToolSelector>(control_state, simulation);
        tools->addItem(tool_selector);
        auto brush_size = create<GUI::NamedContainer>("Brush Size", GUI::Container::Orientation::Vertical);
        auto slider = create<SliderLabel>(10.0f);
        watch(slider, [this, slider](){
            setBrushSize(slider->getValue());
        });
        setBrushSize(slider->getValue());
        brush_size->addItem(slider);
        tools->addItem(brush_size);

        // Add colonies edition tools
        auto colonies = create<ColonyCreator>(simulation, control_state);
        toolbox->addItem(colonies);

        // Add time controls
        auto time_controls = create<TimeController>();
        watch(time_controls, [this, time_controls](){
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

    void updateRenderOptions() const
    {
        renderer->simulation.renderer.render_ants = display_controls->draw_ants;
        renderer->simulation.world.renderer.draw_markers = display_controls->draw_markers;
        renderer->simulation.world.renderer.draw_density = display_controls->draw_density;
    }

    void setBrushSize(float size)
    {
        const int32_t brush_size   = to<int32_t>(size);
        tool_selector->brush_size  = brush_size;
        control_state.brush_radius = to<float>(brush_size);
    }
};

}
