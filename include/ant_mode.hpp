#pragma once
#include <cstdint>


enum class Mode : uint32_t
{
	ToHome = 0,
	ToFood = 1,
	Refill = 2,
	Flee = 3,
	ToHomeNoFood = 4
};