#pragma once
#include <SFML/Graphics.hpp>


const sf::Color FOOD_COLOR(255, 205, 31);


struct Food
{
	Food() = default;

	Food(float x, float y, float r, Marker* maker_ptr = nullptr)
		: position(x, y)
		, radius(r)
		, done(false)
		, marker(maker_ptr)
	{}

	void destroy()
	{
		done = true;
		if (marker) {
			marker->intensity = 50.0f;
			marker->permanent = false;
		}
	}

	bool isDone() const
	{
		return done;
	}

	void render(sf::RenderTarget& target) const
	{
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPosition(position);
		circle.setFillColor(FOOD_COLOR);

		target.draw(circle);
	}

	sf::Vector2f position;
	float radius;
	bool done;
	Marker* marker;
};
