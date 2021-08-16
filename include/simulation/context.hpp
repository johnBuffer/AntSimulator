#pragma once
#include "common/number_generator.hpp"


namespace sim
{

struct Context
{
    float dt = 0.016f;
    RandFloatGenerator rng;
    sf::Clock clock;
    
    Context()
        : rng()
    {
        
    }
};

}
