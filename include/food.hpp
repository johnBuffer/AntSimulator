#pragma once
#include <SFML/Graphics.hpp>


struct Food
{
	Food(float x, float y, float r)
		: position(x, y)
		, radius(r)
	{}

	void render(sf::RenderTarget& target) const
	{
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPosition(position);
		circle.setFillColor(sf::Color::White);

		target.draw(circle);
	}

	const sf::Vector2f position;
	const float radius;
};
