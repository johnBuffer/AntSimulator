#pragma once


const sf::Color TO_HOME_COLOR(122, 105, 199);
const sf::Color TO_FOOD_COLOR(255, 228, 117);


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
			intensity -= 0.5f * dt;
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
				circle.setFillColor(TO_HOME_COLOR);
			}
			else {
				circle.setFillColor(TO_FOOD_COLOR);
			}

			target.draw(circle);
		}
	}

	sf::Vector2f position;
	Type type;

	float intensity;
	bool permanent;
};
