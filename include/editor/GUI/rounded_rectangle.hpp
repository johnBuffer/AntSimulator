#pragma once

#include <SFML/Graphics.hpp>

namespace GUI
{

class RoundedRectangle : public sf::Drawable
{
public:
	RoundedRectangle(float width, float height, float radius, float x, float y)
		: m_width(width)
		, m_height(height)
		, m_radius(radius)
		, m_x(x)
		, m_y(y)
		, m_color(sf::Color::White)
	{}
    
    RoundedRectangle(sf::Vector2f size, sf::Vector2f position, float radius)
        : m_width(size.x)
        , m_height(size.y)
        , m_radius(std::min(0.5f * std::min(m_width, m_height), radius))
        , m_x(position.x)
        , m_y(position.y)
        , m_color(sf::Color::White)
    {}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override
	{
		const float d(2 * m_radius);
		sf::RectangleShape r1(sf::Vector2f(m_width - d, m_height));
		sf::RectangleShape r2(sf::Vector2f(m_width, m_height - d));
		sf::CircleShape round(m_radius);
		r1.setFillColor(m_color);
		r2.setFillColor(m_color);
		r1.setPosition(m_x + m_radius, m_y);
		r2.setPosition(m_x, m_y + m_radius);
        round.setFillColor(m_color);

		target.draw(r1, states);
		target.draw(r2, states);
		round.setPosition(m_x, m_y);
		target.draw(round, states);
		round.setPosition(m_x + m_width - d, m_y);
		target.draw(round, states);
		round.setPosition(m_x + m_width - d, m_y + m_height - d);
		target.draw(round, states);
		round.setPosition(m_x, m_y + m_height - d);
		target.draw(round, states);
	}

	void setFillColor(const sf::Color& color)
	{
		m_color = color;
	}

private:
	float m_width;
	float m_height;
	float m_radius;
	float m_x;
	float m_y;

	sf::Color m_color;
};

}
