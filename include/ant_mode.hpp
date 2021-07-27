#pragma once
#include <cstdint>


enum class Mode : uint32_t
{
	ToHome       = 0,
	ToFood       = 1,
	ToEnemy      = 2,
	Refill       = 3,
	Flee         = 4,
	ToHomeNoFood = 5,
    Dead         = 6
};

enum class FightMode
{
    Fighting,
    ToFight,
    NoFight
};
