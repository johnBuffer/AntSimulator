#pragma once

#include <list>
#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "config.hpp"
#include <iostream>
#include "number_generator.hpp"


struct Direction
{
public:
	Direction(float angle, float rotation_speed = 10.0f)
		: m_angle(angle)
		, m_target_angle(angle)
		, m_rotation_speed(rotation_speed)
	{
		updateVec();
		m_target_vec = m_vec;
	}

	void update(float dt)
	{
		updateVec();

		const sf::Vector2f dir_nrm(-m_vec.y, m_vec.x);

		const float dir_delta = dot(m_target_vec, dir_nrm);
		const float rotation_speed = 10.0f;
		m_angle += rotation_speed * dir_delta * dt;
	}

	void setDirection(const sf::Vector2f& d, bool add_pi)
	{
		m_target_angle = atan(d.y / d.x) + add_pi * PI;
		updateTargetVec();
	}

	void setDirectionNow(const sf::Vector2f& d, bool add_pi)
	{
		setDirection(d, add_pi);
		updateVec();
		m_angle = m_target_angle;
	}

	sf::Vector2f getVec() const
	{
		return m_vec;
	}

	void operator+=(float a)
	{
		m_target_angle += a;
		updateTargetVec();
	}

	void operator=(float a)
	{
		m_target_angle = a;
		updateTargetVec();
	}

	void addNow(float a)
	{
		this->operator+=(a);
		m_angle = m_target_angle;
		updateVec();
	}

private:
	sf::Vector2f m_vec;
	sf::Vector2f m_target_vec;
	float m_angle;
	float m_target_angle;
	float m_rotation_speed;

	void updateVec()
	{
		m_vec.x = cos(m_angle);
		m_vec.y = sin(m_angle);
	}

	void updateTargetVec()
	{
		m_target_vec.x = cos(m_target_angle);
		m_target_vec.y = sin(m_target_angle);
	}
};


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle, uint32_t id_)
		: position(x, y)
		, direction(angle)
		, last_direction_update(RNGf::getUnder(1.0f) * direction_update_period)
		, last_marker(RNGf::getUnder(1.0f) * marker_period)
		, phase(Marker::Type::ToFood)
		, reserve(0.0f)
		, id(id_)
		, liberty_coef(RNGf::getRange(0.0001f, 0.001f))
	{
		reserve = max_reserve;
	}

	void update(const float dt, World& world)
	{
		updatePosition(world, dt);
		if (phase == Marker::ToFood) {
			checkFood(world);
		}

		last_direction_update += dt;
		if (last_direction_update > direction_update_period) {
			findMarker(world);
			direction += getRandRange(direction_noise_range);
			last_direction_update = 0.0f;
		}

		last_marker += dt;
		if (last_marker >= marker_period) {
			addMarker(world);
		}

		direction.update(dt);
	}

	void updatePosition(World& world, const float dt)
	{
		sf::Vector2f v = direction.getVec();
		const sf::Vector2f next_position = position + (dt * move_speed) * direction.getVec();
		if (!world.grid_walls.isEmpty(next_position)) {
			const sf::Vector2f normal = getWallNormal(world, v);
			const float dot = normal.x * v.x + normal.y * v.y;
			v = v - 2 * dot * normal;
			direction.setDirectionNow(v, normal.x == -1.0f);
		}
		
		position += (dt * move_speed) * v;

		position.x = position.x < 0.0f ? Conf::WIN_WIDTH : position.x;
		position.y = position.y < 0.0f ? Conf::WIN_HEIGHT : position.y;

		position.x = position.x > Conf::WIN_WIDTH ? 0.0f : position.x;
		position.y = position.y > Conf::WIN_HEIGHT ? 0.0f : position.y;
	}

	void checkFood(World& world)
	{
		const std::list<Food*> food_spots = world.grid_food.getAllAt(position);
		for (Food* fp : food_spots) {
			if (getLength(position - fp->position) < fp->radius) {
				phase = Marker::ToHome;
				direction.addNow(PI);
				reserve = max_reserve;
				fp->pick();
				return;
			}
		}
	}

	void checkColony(const sf::Vector2f colony_position)
	{
		const float colony_size = 20.0f;
		if (getLength(position - colony_position) < colony_size) {
			if (phase == Marker::ToHome) {
				phase = Marker::ToFood;
				direction.addNow(PI);
			}
			reserve = max_reserve;
		}
	}

	void findMarker(World& world)
	{
		// Init
		const sf::Vector2f dir_vec = direction.getVec();
		const uint32_t grid_cell_size = world.markers.cell_size;
		const float radius = 40.0f;
		const int32_t radius_cell = radius / grid_cell_size;
		const int32_t cell_x = position.x / grid_cell_size;
		const int32_t cell_y = position.y / grid_cell_size;
		const int32_t min_range_x = std::max(1, cell_x - radius_cell);
		const int32_t min_range_y = std::max(1, cell_y - radius_cell);
		const int32_t max_range_x = std::min(int32_t(world.markers.size_width) - 2, cell_x + radius_cell);
		const int32_t max_range_y = std::min(int32_t(world.markers.size_height) - 2, cell_y + radius_cell);
		
		float max_intensity = 0.0f;
		sf::Vector2f max_direction;
		// Sample the markers
		const uint32_t sample_count = 100;
		for (uint32_t i(0); i < sample_count; ++i) {
			const uint32_t sample_x = RNGf::getRange(min_range_x, max_range_x + 1.0f);
			const uint32_t sample_y = RNGf::getRange(min_range_y, max_range_y + 1.0f);
			const sf::Vector2f marker_pos = float(grid_cell_size) * sf::Vector2f(sample_x, sample_y) + sf::Vector2f(RNGf::getUnder(grid_cell_size), RNGf::getUnder(grid_cell_size));
			const sf::Vector2f to_marker = marker_pos - position;
			const float length = getLength(to_marker);
			const sf::Vector2f to_marker_v = to_marker / length;

			const auto& cell = world.markers.getCell(sample_x, sample_y);
			if (length < marker_detection_max_dist) {
				if (dot(to_marker_v, dir_vec) > 0.3f) {
					// Check for food or colony
					if (cell.permanent[phase]) {
						max_direction = to_marker_v;
						break;
					}
					// Check for the most intense marker
					if (cell.intensity[phase] > max_intensity) {
						max_intensity = cell.intensity[phase];
						max_direction = to_marker_v;
					}
					// Randomly choose own path
					if (RNGf::proba(liberty_coef)) {
						break;
					}
				}
			}
		}

		if (max_intensity) {
			direction = getAngle(max_direction);
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

	void render_food(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		if (phase == Marker::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.65f * direction.getVec());
			circle.setFillColor(Conf::FOOD_COLOR);
			target.draw(circle, states);
		}
	}

	sf::Vector2f getWallNormal(World& world, const sf::Vector2f& v) const
	{
		const float cell_size_f = to<float>(world.grid_walls.cell_size);
		const float inv_direction[]{ 1.0f / v.x, 1.0f / v.y };
		const float t_d[]{ std::abs(cell_size_f * inv_direction[0]), std::abs(cell_size_f * inv_direction[1]) };
		const int32_t step[]{v.x >= 0.0f ? 1 : -1, v.y >= 0.0f ? 1 : -1 };
		const sf::Vector2i cell_coords = world.grid_walls.getCellCoords(position);

		float t_max[]{
			((cell_coords.x + (step[0] > 0)) * cell_size_f - position.x) * inv_direction[0],
			((cell_coords.y + (step[1] > 0)) * cell_size_f - position.y) * inv_direction[1]
		};

		if (t_max[0] < t_max[1]) {
			return sf::Vector2f(-step[0], 0.0f);
		}

		return sf::Vector2f(0.0f, -step[1]);
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

	sf::Vector2f position;
	
	Direction direction;

	float last_direction_update;
	float last_marker;
	Marker::Type phase;
	float reserve;
	const uint32_t id;
	float liberty_coef;

	// Parameters
	const float width = 3.0f;
	const float length = 4.7f;
	const float move_speed = 50.0f;
	const float marker_detection_max_dist = 40.0f;
	const float direction_update_period = 0.125f;
	const float marker_period = 0.25f;
	const float max_reserve = 6000.0f;
	const float direction_noise_range = PI * 0.1f;
};
