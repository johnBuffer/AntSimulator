#pragma once
#include "world/world.hpp"
#include "colony/colony.hpp"
#include "config.hpp"
#include "common/viewport_handler.hpp"
#include "common/event_manager.hpp"
#include "event_state.hpp"
#include "render/renderer.hpp"
#include "simulation/world/map_loader.hpp"
#include "simulation/ant/fight_system.hpp"


struct Simulation
{
	civ::Vector<Colony> colonies;
	World world;
	// Render
	Renderer renderer;
	sfev::EventManager ev_manager;
	EventSate ev_state;
	FightSystem fight_system;
	sf::Clock clock;

    explicit
	Simulation(sf::Window& window)
		: world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
		, renderer()
		, ev_manager(window, true)
	{
	}

	void loadMap(const std::string& map_filename)
	{
		MapLoader::loadMap(world, map_filename);
	}

	void selectColony()
	{
		for (const Colony& c : colonies) {
			const sf::Vector2f world_mouse_pos = renderer.vp_handler.getMouseWorldPosition();
			const float length = getLength(world_mouse_pos - c.base.position);
			if (length < c.base.radius) {
				world.renderer.selected_colony = c.id;
				return;
			}
		}
		world.renderer.selected_colony = -1;
	}

	void processEvents()
	{
		ev_manager.processEvents();
	}

	civ::Ref<Colony> createColony(float colony_x, float colony_y)
	{
		// Create the colony object
		const civ::ID colony_id = colonies.emplace_back(colony_x, colony_y, Conf::ANTS_COUNT);
		auto colony_ref = colonies.getRef(colony_id);
        Colony& colony = *colony_ref;
        colony.initialize(to<uint8_t>(colony_id));
		colony.ants_color = Conf::COLONY_COLORS[colony.id];
		// Create colony markers
		for (uint32_t i(0); i < 64; ++i) {
			float angle = float(i) / 64.0f * (2.0f * PI);
			world.addMarker(colony.base.position + 16.0f * sf::Vector2f(cos(angle), sin(angle)), Mode::ToHome, 10.0f, true);
		}
		// Register it for the renderer
		renderer.addColony(colony_ref);
        return colony_ref;
	}

	void update(float dt)
	{
		if (!ev_state.pause) {
            // Mark ants with no more time left as dead
            removeDeadAnts();
			// Update world cells (markers, density, walls)
			world.update(dt);
			// First perform position update and grid registration
			for (Colony& colony : colonies) {
				colony.genericAntsUpdate(dt, world);
			}
			// Then update objectives and world sampling
			for (Colony& colony : colonies) {
				colony.update(dt, world);
			}
			// Search for fights
			fight_system.checkForFights(colonies, world);
			// Update stats
			renderer.updateColoniesStats(dt);
		}
	}
    
    void removeDeadAnts()
    {
        // Mark old ants as dead
        for (Colony& colony : colonies) {
            const uint32_t killed = colony.killWeakAnts(world);
            if (killed) {
                const uint32_t initial_size = to<int32_t>(colony.ants.size());
                renderer.colonies[colony.id].cleanVAs(initial_size - killed, initial_size);
            }
        }
        // Remove them
        for (Colony& colony : colonies) {
            colony.removeDeadAnts();
        }
    }

	void render(sf::RenderTarget& target)
	{
		renderer.render(world, target);
	}

    void removeColony(uint8_t colony_id)
    {
        colonies.erase(colony_id);
        for (auto it = renderer.colonies.begin(); it != renderer.colonies.end(); ++it) {
            if (it->colony_ref.getID() == colony_id) {
                renderer.colonies.erase(it);
                break;
            }
        }
        world.clearMarkers(colony_id);
    }
};
