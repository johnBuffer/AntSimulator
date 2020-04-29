#pragma once

#include <list>
#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "world.hpp"


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle)
		: position(x, y)
		, direction(angle)
		, last_direction_update(0.0f)
		, last_marker(0.0f)
		, phase(Marker::Type::ToFood)
		, colony(x, y)
	{}

	void update(const float dt, World& world)
	{
		updatePosition(dt);

		if (phase == Marker::ToFood) {
			checkFood(world.food);
		}
		else {
			checkColony();
		}

		last_direction_update += dt;
		if (last_direction_update >= direction_update_period) {
			findNewDirection(world.grid.getMarkersAt(position));
		}

		last_marker += dt;
		if (last_marker >= marker_period) {
			addMarker(world);
		}
	}

	void updatePosition(const float dt)
	{
		const float speed = 50.0f;
		position += (dt * speed) * sf::Vector2f(cos(direction), sin(direction));

		position.x = position.x < 0.0f ? 1600.0f : position.x;
		position.y = position.y < 0.0f ? 900.0f : position.y;

		position.x = position.x > 1600.0f ? 0.0f : position.x;
		position.y = position.y > 900.0f ? 0.0f : position.y;
	}

	void checkFood(const Food& food)
	{
		if (getLength(position - food.position) < food.radius) {
			phase = Marker::ToHome;
			direction += PI;
		}
	}

	void checkColony()
	{
		const float colony_size = 50.0f;
		if (getLength(position - colony) < colony_size) {
			phase = Marker::ToFood;
			direction += PI;
		}
	}

	void findNewDirection(const std::list<Marker>* markers)
	{
		float new_angle = 0.0f;

		if (findMarker(markers, &new_angle)) {
			direction = new_angle;
		}
		else {
			const float range = PI * 0.05f;
			direction += getRandRange(range);
		}

		last_direction_update = 0.0f;
	}

	bool findMarker(const std::list<Marker>* markers, float* new_angle)
	{
		if (!markers) {
			return false;
		}

		const float max_dist = 10.0f;
		const float max_angle = 0.25f * PI;

		float markers_count = 0.0f;
		float markers_sum = 0.0f;

		for (const Marker& m : *markers) {
			const sf::Vector2f to_marker = m.position - position;
			if (m.type == phase) {
				if (getLength(to_marker) < max_dist) {
					if (dot(to_marker, sf::Vector2f(cos(direction), sin(direction))) > 0.0f) {
						*new_angle = m.angle;
						return true;
					}
				}
			}
		}

		return markers_count;
	}

	void addMarker(World& world)
	{
		world.grid.addMarker(position, Marker(position, phase == Marker::ToFood ? Marker::ToHome : Marker::ToFood, direction + PI));
		last_marker = 0.0f;
	}

	void render(sf::RenderTarget& target) const
	{
		const float width = 2.0f;
		const float length = 7.0f;
		sf::RectangleShape body(sf::Vector2f(width, length));
		body.setOrigin(width * 0.5f, length * 0.5f);
		body.setPosition(position);
		body.setRotation(direction * 57.2958f + 90.0f);
		body.setFillColor(sf::Color::Green);

		if (phase == Marker::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.5f * sf::Vector2f(cos(direction), sin(direction)));
			circle.setFillColor(sf::Color::White);
			target.draw(circle);
		}

		target.draw(body);
	}

	const sf::Vector2f colony;
	sf::Vector2f position;
	float direction;
	float last_direction_update;
	float last_marker;
	Marker::Type phase;

	const float direction_update_period = 0.5f;
	const float marker_period = 0.25f;
};
