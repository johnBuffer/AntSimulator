#pragma once
#include "simulation/world/world.hpp"
#include "common/number_generator.hpp"


namespace simulation
{

struct Context
{
    float dt = 0.016f;
    RealNumberGenerator<float> rng;
    World world;
    sf::Clock clock;
};

}
