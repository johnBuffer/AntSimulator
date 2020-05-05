#pragma once

#include <list>
#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "config.hpp"
#include <iostream>
#include "sound_player.hpp"


constexpr float max_reserve = 2000.0f;


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle, uint32_t id_)
		: colony(x, y)
		, position(x, y)
		, direction(angle)
		, target_direction(angle)
		, last_direction_update(getRandUnder(100.0f) * 0.01f * direction_update_period)
		, last_marker(getRandUnder(100.0f) * 0.01f * marker_period)
		, phase(Marker::Type::ToFood)
		, reserve(max_reserve)
		, id(id_)
	{
	}

	void update(const float dt, World& world)
	{
		updatePosition(dt);

		checkColony();
		if (phase == Marker::ToFood) {
			checkFood(world);
		}

		last_direction_update += dt;
		if (last_direction_update > direction_update_period) {
			findMarker(world);
			float range = PI * 0.2f;
			target_direction += getRandRange(range);
			last_direction_update = 0.0f;
		}

		last_marker += dt;
		if (last_marker >= marker_period) {
			addMarker(world);
		}

		updateDirection(dt);
	}

	void updateDirection(const float dt)
	{
		const sf::Vector2f dir_vec(cos(direction), sin(direction));
		const sf::Vector2f dir_nrm(-dir_vec.y, dir_vec.x);
		const sf::Vector2f target_dir_vec(cos(target_direction), sin(target_direction));

		const float dir_delta = dot(target_dir_vec, dir_nrm);
		const float rotation_speed = 10.0f;
		direction += rotation_speed * dir_delta * dt;
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
				target_direction += PI;
				direction += PI;
				reserve = max_reserve;
				fp->pick();
				return;
			}
		}
	}

	void checkColony()
	{
		const float colony_size = 20.0f;
		if (getLength(position - colony) < colony_size) {
			if (phase == Marker::ToHome) {
				phase = Marker::ToFood;
				target_direction += PI;
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
			target_direction = getAngle(point / total_intensity - position);
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

	void render(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		const float width = 4.0f;
		const float length = 7.0f;
		/*sf::RectangleShape body(sf::Vector2f(width, length));
		body.setOrigin(width * 0.5f, length * 0.5f);
		body.setPosition(position);
		body.setRotation(direction * 57.2958f + 90.0f);
		body.setFillColor(Conf<>::ANT_COLOR);
		body.setTexture(Conf<>::ANT_TEXTURE);*/

		if (phase == Marker::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.5f * sf::Vector2f(cos(direction), sin(direction)));
			circle.setFillColor(Conf<>::FOOD_COLOR);
			target.draw(circle, states);
		}
	}

	void render_in(sf::VertexArray& va, const uint32_t index) const
	{
		const float width = 2.0f;
		const float length = 3.5f;

		const sf::Vector2f dir_vec(cos(direction), sin(direction));
		const sf::Vector2f nrm_vec(-dir_vec.y, dir_vec.x);

		va[index + 0].position = position - width * nrm_vec + length * dir_vec;
		va[index + 1].position = position + width * nrm_vec + length * dir_vec;
		va[index + 2].position = position + width * nrm_vec - length * dir_vec;
		va[index + 3].position = position - width * nrm_vec - length * dir_vec;
	}

	sf::Vector2f colony;
	sf::Vector2f position;
	
	float direction;
	float target_direction;

	float last_direction_update;
	float last_marker;
	Marker::Type phase;
	float reserve = 500.0f;
	const uint32_t id;

	const float direction_update_period = 0.25f;
	const float marker_period = 0.25f;
};
