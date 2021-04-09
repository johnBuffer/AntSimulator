#pragma once
#include "config.hpp"
#include <iostream>


struct Marker
{
	enum Type {
		ToHome = 0,
		ToFood = 1
	};

	Marker() = default;
	Marker(const sf::Vector2f& pos, Type type_, float intensity_, bool permanent_ = false)
		: position(pos)
		, intensity(intensity_)
		, type(type_)
		, permanent(permanent_)
	{}

	void update(const float dt)
	{
		if (!permanent) {
			intensity -= 1.0f * dt;
		}
	}

	void destroy()
	{
		intensity = -1.0f;
	}

	bool isDone() const
	{
		return intensity < 0.0f;
	}

	sf::Vector2f position;
	Type type;

	float intensity;
	bool permanent;
};
