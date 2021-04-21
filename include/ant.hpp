#pragma once

#include <list>
#include "world.hpp"
#include "config.hpp"
#include "direction.hpp"
#include "number_generator.hpp"
#include "ant_mode.hpp"

#include <iostream>


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle)
		: position(x, y)
		, direction(angle)
		, last_direction_update(RNGf::getUnder(1.0f) * direction_update_period)
		, last_marker(RNGf::getUnder(1.0f) * marker_period)
		, phase(Mode::ToFood)
		, liberty_coef(RNGf::getRange(0.0001f, 0.001f))
		, hits(0)
		, markers_count(0.0f)
	{
	}

	void update(const float dt, World& world)
	{
		updatePosition(world, dt);
		if (phase == Mode::ToFood) {
			checkFood(world);
		}

		last_direction_update += dt;
		if (last_direction_update > direction_update_period) {
			findMarker(world, dt);
			direction += RNGf::getFullRange(direction_noise_range);
			last_direction_update = 0.0f;
		}

		last_marker += dt;
		if (last_marker >= marker_period) {
			addMarker(world);
		}

		direction.update(dt);
	}

	void updatePosition(World& world, float dt)
	{
		sf::Vector2f v = direction.getVec();
		const sf::Vector2f next_position = position + (dt * move_speed) * v;
		if (world.markers.get(next_position).wall) {
			++hits;
			const float new_angle = RNGf::getFullRange(PI);
			const sf::Vector2f new_direction(cos(new_angle), sin(new_angle));
			direction.setDirectionNow(new_direction);
			if (hits > 8) {
				// Bad but nothing better for now
				position = Conf::COLONY_POSITION;
			}
		}
		else {
			hits = 0;
			position += (dt * move_speed) * v;
			// Ants outside the map go back to home
			position.x = (position.x < 0.0f || position.x > Conf::WIN_WIDTH) ? Conf::COLONY_POSITION.x : position.x;
			position.y = (position.y < 0.0f || position.y > Conf::WIN_HEIGHT) ? Conf::COLONY_POSITION.y : position.y;
		}
	}

	void checkFood(World& world)
	{
		if (world.markers.isOnFood(position)) {
			phase = Mode::ToHome;
			direction.addNow(PI);
			world.markers.pickFood(position);
			markers_count = 0.0f;
			return;
		}
	}

	void checkColony(const sf::Vector2f colony_position)
	{
		if (getLength(position - colony_position) < colony_size) {
			if (phase == Mode::ToHome) {
				phase = Mode::ToFood;
				direction.addNow(PI);
			}
			markers_count = 0.0f;
		}
	}

	void findMarker(World& world, float dt)
	{
		// Init
		const float sample_angle_range = PI * 0.25f;
		const float current_angle = direction.getCurrentAngle();
		float max_intensity = 0.0f;
		sf::Vector2f max_direction;
		MarkerCell* max_cell = nullptr;
		// Sample the world
		const uint32_t sample_count = 24;
		for (uint32_t i(sample_count); i--;) {
			// Get random point in range
			const float sample_angle = current_angle + RNGf::getRange(sample_angle_range);
			const float distance = RNGf::getUnder(marker_detection_max_dist);
			const sf::Vector2f to_marker(cos(sample_angle), sin(sample_angle));
			auto* cell = world.markers.getSafe(position + distance * to_marker);
			// Check cell
			if (!cell) {
				continue;
			}
			// Check for food or colony
			if (cell->permanent[static_cast<uint32_t>(phase)]) {
				max_direction = to_marker;
				break;
			}
			// Check for the most intense marker
			if (cell->intensity[static_cast<uint32_t>(phase)] > max_intensity) {
				max_intensity = cell->intensity[static_cast<uint32_t>(phase)];
				max_direction = to_marker;
				max_cell = cell;
			}
			// Randomly choose own path
			if (RNGf::proba(liberty_coef)) {
				break;
			}
		}
		// Update direction
		if (max_intensity) {
			if (RNGf::proba(0.3f)) {
				max_cell->intensity[static_cast<uint32_t>(phase)] *= 0.99f;
			}
			direction = getAngle(max_direction);
		}
	}

	void addMarker(World& world)
	{
		markers_count += marker_period;
		const float coef = 0.01f;
		const float intensity = 1000.0f * exp(-coef * markers_count);
		world.addMarker(position, phase == Mode::ToFood ? Mode::ToHome : Mode::ToFood, intensity);
		last_marker = 0.0f;
	}

	void render_food(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		if (phase == Mode::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.65f * direction.getVec());
			circle.setFillColor(Conf::FOOD_COLOR);
			target.draw(circle, states);
		}
	}

	void render_in(sf::VertexArray& va, const uint32_t index) const
	{
		const sf::Vector2f dir_vec(direction.getVec());
		const sf::Vector2f nrm_vec(-dir_vec.y, dir_vec.x);

		va[index + 0].position = position - width * nrm_vec + length * dir_vec;
		va[index + 1].position = position + width * nrm_vec + length * dir_vec;
		va[index + 2].position = position + width * nrm_vec - length * dir_vec;
		va[index + 3].position = position - width * nrm_vec - length * dir_vec;
	}

	// Parameters
	const float width = 3.0f;
	const float length = 4.7f;
	const float move_speed = 50.0f;
	const float marker_detection_max_dist = 40.0f;
	const float direction_update_period = 0.125f;
	const float marker_period = 0.125f;
	const float direction_noise_range = PI * 0.1f;
	const float colony_size = 20.0f;

	uint32_t hits;

	Mode phase;
	sf::Vector2f position;
	Direction direction;

	float last_direction_update;
	float markers_count;
	float last_marker;
	float liberty_coef;
};
