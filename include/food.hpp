#pragma once
#include <SFML/Graphics.hpp>


struct Food
{
	Food() = default;

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

	sf::Vector2f position;
	float radius;
};
