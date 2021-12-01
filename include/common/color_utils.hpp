#pragma once
#include <SFML/Graphics.hpp>
#include "common/math.hpp"


struct ColorUtils
{

	template<typename T>
	static sf::Color createColor(T r, T g, T b)
	{
		return sf::Color{ to<uint8_t>(r), to<uint8_t>(g), to<uint8_t>(b) };
	}

	static sf::Color getRainbow(float t)
	{
		const float r = sin(t);
		const float g = sin(t + 0.33f * 2.0f * Math::PI);
		const float b = sin(t + 0.66f * 2.0f * Math::PI);
		return createColor(255 * r * r, 255 * g * g, 255 * b * b);
	}
};
