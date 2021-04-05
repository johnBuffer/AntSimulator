#pragma once
#include <SFML/Graphics.hpp>
#include "config.hpp"


struct Food
{
	Food() = default;

	Food(float x, float y, float r, float quantity_)
		: position(x, y)
		, radius(r)
		, quantity(quantity_)
	{}

	void pick()
	{
		quantity -= 1.0f;
	}

	bool isDone() const
	{
		return quantity <= 0.0f;
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPosition(position);
		circle.setFillColor(Conf<>::FOOD_COLOR);

		target.draw(circle, states);
	}

	sf::Vector2f position;
	float radius;
	float quantity;
};
