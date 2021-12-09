#pragma once
#include <SFML/Graphics.hpp>
#include "common/math.hpp"
#include "common/number_generator.hpp"


struct ColorUtils
{

	template<typename T>
	static sf::Color createColor(T r, T g, T b)
	{
		return sf::Color{ std::min(uint8_t{255}, std::max(uint8_t{0}, to<uint8_t>(r))),
                          std::min(uint8_t{255}, std::max(uint8_t{0}, to<uint8_t>(g))),
                          std::min(uint8_t{255}, std::max(uint8_t{0}, to<uint8_t>(b))) };
	}

	static sf::Color getRainbow(float t)
	{
		const float r = sin(t);
		const float g = sin(t + 0.33f * 2.0f * Math::PI);
		const float b = sin(t + 0.66f * 2.0f * Math::PI);
		return createColor(255 * r * r, 255 * g * g, 255 * b * b);
	}

    static sf::Color getRandomColor()
    {
        return createColor(RNGf::getUnder(255.0f), RNGf::getUnder(255.0f), RNGf::getUnder(255.0f));
    }

    static sf::Color getDesaturated(sf::Color color, float ratio)
    {
        const float l = 0.3f * color.r + 0.6f * color.g + 0.1f * color.b;
        float new_r = color.r + ratio * (l - color.r);
        float new_g = color.g + ratio * (l - color.g);
        float new_b = color.b + ratio * (l - color.b);
        return createColor(new_r, new_g, new_b);
    }
};
