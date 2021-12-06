#pragma once
#include "common/viewport_handler.hpp"
#include "common/utils.hpp"
#include "simulation/config.hpp"
#include "common/event_manager.hpp"
#include "simulation/world/world.hpp"
#include "colony_renderer.hpp"


struct Renderer
{
	bool render_ants = true;
	ViewportHandler vp_handler;
	civ::Vector<ColonyRenderer> colonies;
	
	Renderer()
		: vp_handler({ to<float>(Conf::WIN_WIDTH), to<float>(Conf::WIN_HEIGHT) })
	{
	}

	void toggleRenderAnts()
	{
		render_ants = !render_ants;
	}

	void addColony(civ::Ref<Colony>& colony)
	{
		colonies.emplace_back(colony);
	}

	void updateColoniesStats(float dt)
	{
		for (ColonyRenderer& colony : colonies) {
			colony.updatePopulation(dt);
		}
	}

	void render(World& world, sf::RenderTarget& target)
	{
		const sf::RenderStates& rs = vp_handler.getRenderState();
		// Draw background;
		sf::RectangleShape ground(sf::Vector2f(world.size.x, world.size.y));
		ground.setFillColor(sf::Color::Black);
		target.draw(ground, rs);
		// Draw world
		world.render(target, rs);
		// Draw ants and colonies
		for (ColonyRenderer& colony : colonies) {
			if (render_ants) {
				colony.renderAnts(target, rs);
			}
			colony.render(target, rs);
		}
//		// Render UI elements
//		for (ColonyRenderer& colony : colonies) {
//			colony.render_charts(target);
//		}
	}
};


