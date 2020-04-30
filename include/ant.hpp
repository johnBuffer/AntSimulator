#pragma once

#include <list>
#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "config.hpp"
#include <iostream>


constexpr float max_reserve = 2000.0f;


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle, uint32_t id_)
		: colony(x, y)
		, position(x, y)
		, direction(angle)
		, last_direction_update(getRandUnder(direction_update_period))
		, last_marker(0.0f)
		, phase(Marker::Type::ToFood)
		, reserve(max_reserve)
		, id(id_)
	{}

	void update(const float dt, World& world)
	{
		updatePosition(dt);

		checkColony();
		if (phase == Marker::ToFood) {
			checkFood(world);
		}

		last_direction_update += dt;
		if (last_direction_update > direction_update_period) {
			float range = PI * 0.08f;
			direction += getRandRange(range);
			last_direction_update = 0.0f;
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

		position.x = position.x < 0.0f ? Conf<>::WIN_WIDTH : position.x;
		position.y = position.y < 0.0f ? Conf<>::WIN_HEIGHT : position.y;

		position.x = position.x > Conf<>::WIN_WIDTH ? 0.0f : position.x;
		position.y = position.y > Conf<>::WIN_HEIGHT ? 0.0f : position.y;
	}

	void checkFood(World& world)
	{
		const std::list<Food*> food_spots = world.grid_food.getAllAt(position);
		for (Food* fp : food_spots) {
			if (getLength(position - fp->position) < fp->radius) {
				phase = Marker::ToHome;
				direction += PI;
				reserve = max_reserve;
				fp->pick();
			}
		}
	}

	void checkColony()
	{
		const float colony_size = 20.0f;
		if (getLength(position - colony) < colony_size) {
			if (phase == Marker::ToHome) {
				phase = Marker::ToFood;
				direction += PI;
			}
			reserve = max_reserve;
		}
	}

	void findNewDirection(World& world)
	{
		findMarker(world);
	}

	void findMarker(World& world)
	{
		std::list<Marker*> markers;
		if (phase == Marker::ToFood) {
			markers = world.grid_markers_food.getAllAt(position);
		}
		else {
			markers = world.grid_markers_home.getAllAt(position);
		}

		const float max_dist = 40.0f;

		float total_intensity = 0.0f;
		sf::Vector2f point(0.0f, 0.0f);

		for (Marker* mp : markers) {
			Marker& m = *mp;
			const sf::Vector2f to_marker = m.position - position;
			const float length = getLength(to_marker);

			if (length < max_dist) {
				if (m.type == phase) {
					if (dot(to_marker, sf::Vector2f(cos(direction), sin(direction))) > 0.0f) {
						total_intensity += m.intensity;
						point += m.intensity * m.position;
					}
				}
			}
		}

		if (total_intensity) {
			direction = getAngle(point / total_intensity - position);
		}
	}

	void addMarker(World& world)
	{
		if (reserve > 1.0f) {
			world.addMarker(Marker(position, phase == Marker::ToFood ? Marker::ToHome : Marker::ToFood, reserve * 0.02f));
			reserve *= 0.98f;
		}

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
		body.setFillColor(Conf<>::ANT_COLOR);

		if (phase == Marker::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.5f * sf::Vector2f(cos(direction), sin(direction)));
			circle.setFillColor(Conf<>::FOOD_COLOR);
			target.draw(circle);
		}

		target.draw(body);
	}

	sf::Vector2f colony;
	sf::Vector2f position;
	float direction;
	float last_direction_update;
	float last_marker;
	Marker::Type phase;
	float reserve = 500.0f;
	const uint32_t id;

	const float direction_update_period = 0.125f;
	const float marker_period = 0.25f;
};
