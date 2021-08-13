#pragma once
#include "world/world.hpp"
#include "colony/colony.hpp"
#include "config.hpp"
#include "simulation/world/map_loader.hpp"
#include "simulation/ant/fight_system.hpp"
#include "simulation_context.hpp"


namespace sim
{

struct Simulation
{
    RealNumberGenerator<float> gen;
    std::vector<Colony> colonies;
    World world;
    FightSystem fight_system;
};

}

