#pragma once
#include <SFML/Graphics.hpp>


struct Graphic
{
	mutable sf::VertexArray va;
	std::vector<float> values;
	float max_value;
	float width;
	float height;
	float x, y;

	sf::Color color;

	uint32_t current_index;
	bool full;

	Graphic(uint32_t values_count, sf::Vector2f size, sf::Vector2f position)
		: va(sf::Quads, values_count * 4)
		, values(values_count, 0.0f)
		, max_value(0.0f)
		, width(size.x)
		, height(size.y)
		, x(position.x)
		, y(position.y)
		, color(sf::Color::White)
		, current_index(0)
		, full(false)
	{
	}

	void addValue(float value)
	{
		const uint64_t size = values.size();
		values[(current_index++) % size] = value;
		max_value = std::max(max_value, value);
		if (current_index == size) {
			full = true;
		}
	}

	void next()
	{
		++current_index;
		if (current_index == values.size()) {
			full = true;
			current_index = 0;
		}
	}

	void setLastValue(float value)
	{
		const uint64_t size = values.size();
		values[current_index % size] = value;
		max_value = std::max(max_value, value);
	}

	void render(sf::RenderTarget& target) const
	{
		const uint64_t size = values.size();
		const float bw = width / float(size);
		const float hf = height / max_value;
		for (uint64_t i(0); i < size; ++i) {
			const uint64_t index = full ? (current_index + 1 + i) % size : i;
			const float bh = values[index] * hf;
			va[4 * i + 0].position = sf::Vector2f(x + i * bw      , y + height);
			va[4 * i + 1].position = sf::Vector2f(x + (i + 1) * bw, y + height);
			va[4 * i + 2].position = sf::Vector2f(x + (i + 1) * bw, y + height - bh);
			va[4 * i + 3].position = sf::Vector2f(x + i * bw      , y + height - bh);

			va[4 * i + 0].color = color;
			va[4 * i + 1].color = color;
			va[4 * i + 2].color = color;
			va[4 * i + 3].color = color;
		}

		target.draw(va);
	}
};


