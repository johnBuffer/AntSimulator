#pragma once
#include <SFML/Graphics.hpp>
#include "config.hpp"
#include "utils.hpp"
#include "colony.hpp"


struct CircularGauge
{
	sf::Vector2f position;
	sf::Color color;
	float outer_radius;
	float inner_radius;
	float min_value;
	float max_value;
	float current_value;

	uint32_t quality = 32;

	CircularGauge(sf::Vector2f pos, float inner_r, float outer_r, const sf::Color& color_)
		: position(pos)
		, color(color_)
		, outer_radius(outer_r)
		, inner_radius(inner_r)
		, min_value(0.0f)
		, max_value(1.0f)
	{}

	void render(sf::RenderTarget& target, sf::RenderStates& states)
	{
		const float ratio = (current_value - min_value) / (max_value - min_value);
		const float max_angle = 2.0f * PI * ratio;
		sf::VertexArray va(sf::TriangleStrip, 2 * quality);
		for (uint32_t i(0); i < quality; ++i) {
			const float angle = max_angle * (i / float(quality - 1));
			const sf::Vector2f vec(-cos(angle), -sin(angle));
			const sf::Vector2f point_1 = position + inner_radius * vec;
			const sf::Vector2f point_2 = position + outer_radius * vec;
			va[2 * i].position = point_1;
			va[2 * i + 1].position = point_2;
		}
		target.draw(va, states);
	}
};


struct ColonyRenderer
{
	sf::Font font;
	sf::Text text;

	ColonyRenderer()
	{
		font.loadFromFile("res/font.ttf");
	}

	void render(const Colony& colony, sf::RenderTarget& target, sf::RenderStates& states)
	{
		const float size = colony.base.radius;
		sf::CircleShape circle(size);
		circle.setOrigin(size, size);
		circle.setPosition(colony.base.position);
		circle.setFillColor(Conf::COLONY_COLOR);
		target.draw(circle, states);

		CircularGauge food_gauge(colony.base.position, 12.0f, 17.0f, sf::Color::White);
		food_gauge.max_value = colony.base.max_food;
		food_gauge.current_value = colony.base.food;
		food_gauge.render(target, states);

		colony.population.render(target);
	}
};
