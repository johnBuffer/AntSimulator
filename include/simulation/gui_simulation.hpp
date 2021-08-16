#pragma once
#include "world/world.hpp"
#include "colony/colony.hpp"
#include "config.hpp"
#include "common/viewport_handler.hpp"
#include "common/event_manager.hpp"
#include "event_state.hpp"
#include "render/renderer.hpp"
#include "simulation/offscreen_simulation.hpp"


namespace sim
{

struct GUISimulation
{
    Simulation simulation;
	// Render
	Renderer renderer;
	sfev::EventManager ev_manager;
	EventSate ev_state;

    GUISimulation(sf::Window& window)
		: renderer()
		, ev_manager(window, true)
	{
		initEventCallbacks();
	}

	void initEventCallbacks()
	{
		ev_manager.addMousePressedCallback(sf::Mouse::Right, [&](sfev::CstEv) {
			selectColony();
			simulation.world.renderer.draw_to_enemies = false;
		});

		ev_manager.addMousePressedCallback(sf::Mouse::Middle, [&](sfev::CstEv) {
			selectColony();
			simulation.world.renderer.draw_to_enemies = true;
		});

		ev_manager.addEventCallback(sf::Event::Closed, [this](sfev::CstEv) {
			ev_manager.getWindow().close();
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::Escape, [this](sfev::CstEv) {
			ev_manager.getWindow().close();
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::P, [this](sfev::CstEv) {
			ev_state.pause = !ev_state.pause;
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::A, [this](sfev::CstEv) {
			renderer.toggleRenderAnts();
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::S, [this](sfev::CstEv) {
			ev_state.fullspeed = !ev_state.fullspeed;
			ev_manager.getWindow().setFramerateLimit(ev_state.fullspeed ? 2000 : 60);
		});
        
        // Viewport Handler controls
        ev_manager.addMousePressedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
            ev_state.clicking = true;
            renderer.vp_handler.click(ev_manager.getFloatMousePosition());
        });

        ev_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
            ev_state.clicking = false;
            renderer.vp_handler.unclick();
        });

        ev_manager.addEventCallback(sf::Event::MouseMoved, [&](sfev::CstEv) {
            renderer.vp_handler.setMousePosition(ev_manager.getFloatMousePosition());
        });
        
        ev_manager.addKeyPressedCallback(sf::Keyboard::R, [this](sfev::CstEv) {
            renderer.vp_handler.reset();
        });
        
        ev_manager.addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e) {
            renderer.vp_handler.wheelZoom(e.mouseWheelScroll.delta);
        });
	}

	void selectColony()
	{
		for (const Colony& c : simulation.colonies) {
			const sf::Vector2f world_mouse_pos = renderer.vp_handler.getMouseWorldPosition();
			const float length = getLength(world_mouse_pos - c.base.position);
			if (length < c.base.radius) {
				simulation.world.renderer.selected_colony = c.id;
				return;
			}
		}
		simulation.world.renderer.selected_colony = -1;
	}

	void processEvents()
	{
		ev_manager.processEvents();
	}
    
    void createColony(float x, float y)
    {
        simulation.createColony(x, y);
        // Register it for the renderer
        renderer.addColony(simulation.colonies.back());
    }
    
    void loadMap(const std::string& filename)
    {
        simulation.loadMap(filename);
    }

	void update()
	{
		if (!ev_state.pause) {
            simulation.update();
			// Update stats
			renderer.updateColoniesStats(simulation.context.dt);
            cleanVAs();
		}
	}
    
    void cleanVAs()
    {
        for (const Colony& c : simulation.colonies) {
            renderer.colonies[c.id].cleanVAs(c.ants.size());
        }
    }
    
	void render(sf::RenderTarget& target)
	{
		renderer.render(simulation.world, target);
	}
};

}

