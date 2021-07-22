#pragma once
#include "world.hpp"
#include "colony.hpp"
#include "config.hpp"
#include "viewport_handler.hpp"
#include "event_manager.hpp"
#include "event_state.hpp"
#include "renderer.hpp"
#include "map_loader.hpp"
#include "fight_system.hpp"


struct Simulation
{
	std::vector<Colony> colonies;
	World world;
	// Render
	Renderer renderer;
	sfev::EventManager ev_manager;
	EventSate ev_state;
	FightSystem fight_system;

	Simulation(sf::Window& window)
		: world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
		, renderer()
		, ev_manager(window, true)
	{
		colonies.reserve(8);
		initEventCallbacks();
	}

	void loadMap(const std::string& map_filename)
	{
		MapLoader::loadMap(world, map_filename);
	}

	void initEventCallbacks()
	{
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

		ev_manager.addEventCallback(sf::Event::Closed, [this](sfev::CstEv) {
			ev_manager.getWindow().close();
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::Escape, [this](sfev::CstEv) {
			ev_manager.getWindow().close();
		});

		ev_manager.addEventCallback(sf::Event::MouseWheelScrolled, [&](sfev::CstEv e) {
			renderer.vp_handler.wheelZoom(e.mouseWheelScroll.delta);
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::P, [this](sfev::CstEv) {
			ev_state.pause = !ev_state.pause;
		});

		ev_manager.addKeyPressedCallback(sf::Keyboard::S, [this](sfev::CstEv) {
			ev_state.fullspeed = !ev_state.fullspeed;
			ev_manager.getWindow().setFramerateLimit(60 * (!ev_state.fullspeed));
		});
	}

	void processEvents()
	{
		ev_manager.processEvents();
	}

	void createColony(float colony_x, float colony_y)
	{
		// Create the colony object
		const uint8_t colony_id = to<uint8_t>(colonies.size());
		colonies.emplace_back(colony_x, colony_y, Conf::ANTS_COUNT, colony_id);
		Colony& colony = colonies.back();
		colony.ants_color = Conf::COLONY_COLORS[colony.id];
		// Create colony markers
		for (uint32_t i(0); i < 64; ++i) {
			float angle = float(i) / 64.0f * (2.0f * PI);
			world.addMarker(colony.base.position + 16.0f * sf::Vector2f(cos(angle), sin(angle)), Mode::ToHome, 10.0f, true);
		}
		// Register it for the renderer
		renderer.addColony(colony);
	}

	void update(float dt)
	{
		if (!ev_state.pause) {
			// Update world cells (markers, density, walls)
			world.update(dt);
			// Update ants
			for (Colony& colony : colonies) {
				colony.update(dt, world);
			}
			// Search for fights
			fight_system.checkForFights(colonies, world);
			// Then remove dead ones
			for (Colony& colony : colonies) {
				colony.removeDeadAnts();
			}
			// Update stats
			renderer.updateColoniesStats(dt);
		}
	}

	void render(sf::RenderTarget& target)
	{
		renderer.render(world, target);
	}
};

