#pragma once


struct Marker
{
	enum Type {
		ToHome,
		ToFood
	};

	Marker() = default;
	Marker(const sf::Vector2f& pos, Type type_, float angle_)
		: position(pos)
		, time(0.0f)
		, type(type_)
		, angle(angle_)
	{}

	void update(const float dt)
	{
		time += dt;
	}

	bool isDone() const
	{
		return time >= max_time;
	}

	void render(sf::RenderTarget& target) const
	{
		const float radius = 2.0f;
		const float time_ratio = 1.0f - time / max_time;
		sf::CircleShape circle(radius);
		circle.setOrigin(radius, radius);
		circle.setPosition(position);

		if (type == ToHome) {
			const uint8_t red = static_cast<uint8_t>(255.0f * time_ratio);
			circle.setFillColor(sf::Color(red, 0, 0));
		}
		else {
			const uint8_t red = static_cast<uint8_t>(255.0f * time_ratio);
			const uint8_t green = static_cast<uint8_t>(255.0f * time_ratio);
			circle.setFillColor(sf::Color(red, green, 0));
		}

		target.draw(circle);
	}

	sf::Vector2f position;
	float angle;
	Type type;
	float time;
	const float max_time = 40.0f;
};
