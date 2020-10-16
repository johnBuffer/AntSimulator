#pragma once

#include <list>
#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"
#include "world.hpp"
#include "config.hpp"
#include <iostream>


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
		, last_direction_update(getRandUnder(100.0f) * 0.01f * direction_update_period)
		, last_marker(getRandUnder(100.0f) * 0.01f * marker_period)
		, phase(Marker::Type::ToFood)
		, reserve(0.0f)
		, id(id_)
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
		std::list<Marker*> markers = world.getGrid(phase).getAllAt(position);

		float total_intensity = 0.0f;
		sf::Vector2f point(0.0f, 0.0f);

		const sf::Vector2f dir_vec = direction.getVec();

		for (Marker* mp : markers) {
			const Marker& m = *mp;
			const sf::Vector2f to_marker = m.position - position;
			const float length = getLength(to_marker);

			if (length < marker_detection_max_dist) {
				if (dot(to_marker, dir_vec) > 0.0f) {
					total_intensity += m.intensity;
					point += m.intensity * m.position;
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

	void render_food(sf::RenderTarget& target, const sf::RenderStates& states) const
	{
		if (phase == Marker::ToHome) {
			const float radius = 2.0f;
			sf::CircleShape circle(radius);
			circle.setOrigin(radius, radius);
			circle.setPosition(position + length * 0.5f * direction.getVec());
			circle.setFillColor(Conf<>::FOOD_COLOR);
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

	// Parameters
	const float width = 2.0f;
	const float length = 3.5f;
	const float move_speed = 50.0f;
	const float marker_detection_max_dist = 40.0f;
	const float direction_update_period = 0.125f;
	const float marker_period = 0.25f;
	const float max_reserve = 6000.0f;
	const float direction_noise_range = PI * 0.1f;
};
