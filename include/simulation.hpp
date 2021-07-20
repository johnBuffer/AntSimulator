#pragma once
#include "world.hpp"
#include "colony.hpp"
#include "config.hpp"
#include "viewport_handler.hpp"
#include "event_manager.hpp"
#include "event_state.hpp"


struct Simulation
{
	std::vector<Colony> colonies;
	World world;
	// Render
	ViewportHandler vp_handler;
	sfev::EventManager ev_manager;
	EventSate ev_state;

	Simulation(sf::Window& window)
		: world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
		, vp_handler({ to<float>(Conf::WIN_WIDTH), to<float>(Conf::WIN_HEIGHT) })
		, ev_manager(window, true)
	{

	}

	void initEventCallbacks()
	{
		ev_manager.addMousePressedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
			ev_state.clicking = true;
			vp_handler.click(ev_manager.getFloatMousePosition());
		});

		ev_manager.addMouseReleasedCallback(sf::Mouse::Left, [&](sfev::CstEv) {
			ev_state.clicking = false;
			vp_handler.unclick();
		});

		ev_manager.addEventCallback(sf::Event::MouseMoved, [&](sfev::CstEv) {
			vp_handler.setMousePosition(ev_manager.getFloatMousePosition());
		});
	}

	void processEvents()
	{
		ev_manager.processEvents();
	}

	void createColony(float colony_x, float colony_y)
	{
		colonies.emplace_back(colony_x, colony_y, Conf::ANTS_COUNT, to<uint8_t>(colonies.size()));
		Colony& colony = colonies.back();
		for (uint32_t i(0); i < 64; ++i) {
			float angle = float(i) / 64.0f * (2.0f * PI);
			world.addMarker(colony.base.position + 16.0f * sf::Vector2f(cos(angle), sin(angle)), Mode::ToHome, 10.0f, true);
		}
	}

	void update(float dt)
	{
		for (Colony& colony : colonies) {
			colony.update(dt, world);
		}
		world.update(dt);
	}
};

