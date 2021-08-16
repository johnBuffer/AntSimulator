#pragma once
#include "world/world.hpp"
#include "colony/colony.hpp"
#include "config.hpp"
#include "simulation/world/map_loader.hpp"
#include "simulation/ant/fight_system.hpp"
#include "context.hpp"


namespace sim
{

struct Simulation
{
    std::vector<Colony> colonies;
    World world;
    FightSystem fight_system;
    Context context;
    
    Simulation()
        : world(Conf::WORLD_WIDTH, Conf::WORLD_HEIGHT)
    {
        colonies.reserve(8);
    }
    
    void loadMap(const std::string& map_filename)
    {
        MapLoader::loadMap(world, map_filename);
    }
    
    void createColony(float colony_x, float colony_y)
    {
        // Create the colony object
        const uint8_t colony_id = to<uint8_t>(colonies.size());
        colonies.emplace_back(colony_x, colony_y, Conf::ANTS_COUNT, colony_id, context);
        Colony& colony = colonies.back();
        colony.ants_color = Conf::COLONY_COLORS[colony.id];
        // Create colony markers
        for (uint32_t i(0); i < 64; ++i) {
            float angle = float(i) / 64.0f * (2.0f * PI);
            world.addMarker(colony.base.position + 16.0f * sf::Vector2f(cos(angle), sin(angle)), Mode::ToHome, 10.0f, true);
        }
    }
    
    void update()
    {
        removeDeadAnts();
        // Update world cells (markers, density, walls)
        world.update(context.dt);
        // First perform position update and grid registration
        for (Colony& colony : colonies) {
            colony.genericAntsUpdate(world, context);
        }
        // Then update objectives and world sampling
        for (Colony& colony : colonies) {
            colony.update(world, context);
        }
        // Search for fights
        fight_system.checkForFights(colonies, world);
    }

    void removeDeadAnts()
    {
        // Mark old ants as dead
        for (Colony& colony : colonies) {
            colony.killWeakAnts(world);
        }
    }

};

}

