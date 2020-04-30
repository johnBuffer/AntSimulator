#pragma once
#include "config.hpp"


struct Marker
{
	enum Type {
		ToHome,
		ToFood
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

	void render(sf::RenderTarget& target) const
	{
		if (intensity > 0.0f && !permanent) {
			const float radius = intensity / 10.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position);

			if (type == ToHome) {
				circle.setFillColor(Conf<>::TO_HOME_COLOR);
			}
			else {
				circle.setFillColor(Conf<>::TO_FOOD_COLOR);
			}

			target.draw(circle);
		}
	}

	void render_in(sf::VertexArray& va, const uint32_t index) const
	{
		if (!permanent) {
			const float radius = intensity / 10.0f;
			
			sf::Color color = (type == ToHome) ? Conf<>::TO_HOME_COLOR : Conf<>::TO_FOOD_COLOR;

			va[index + 0].position = position + sf::Vector2f(radius, 0.0f);
			va[index + 1].position = position + sf::Vector2f(0.0f, radius);
			va[index + 2].position = position + sf::Vector2f(-radius, 0.0f);
			va[index + 3].position = position + sf::Vector2f(0.0f, -radius);

			va[index + 0].color = color;
			va[index + 1].color = color;
			va[index + 2].color = color;
			va[index + 3].color = color;
		}
	}

	sf::Vector2f position;
	Type type;

	float intensity;
	bool permanent;
};
