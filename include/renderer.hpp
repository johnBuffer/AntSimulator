#pragma once
#include "viewport_handler.hpp"
#include "utils.hpp"
#include "config.hpp"
#include "event_manager.hpp"
#include "world.hpp"


struct Renderer
{
	ViewportHandler vp_handler;
	std::vector<ColonyRenderer> colonies;
	
	Renderer()
		: vp_handler({ to<float>(Conf::WIN_WIDTH), to<float>(Conf::WIN_HEIGHT) })
	{
		colonies.reserve(8);
	}

	void addColony(Colony& colony)
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
			colony.renderAnts(target, rs);
			colony.render(target, rs);
		}
	}
};


