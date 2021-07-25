#pragma once

#include <list>
#include "world.hpp"
#include "config.hpp"
#include "direction.hpp"
#include "number_generator.hpp"
#include "ant_mode.hpp"
#include "cooldown.hpp"
#include "colony_base.hpp"
#include "index_vector.hpp"

#include <iostream>


struct Ant
{
	// Parameters
	float width = 3.0f;
	float length = 4.7f;
	float move_speed = 40.0f;
	float marker_detection_max_dist = 40.0f;
	float direction_update_period = 0.25f;
	float marker_period = 0.25f;
	float direction_noise_range = PI * 0.02f;
	float repellent_period = 32.0f;

	Mode phase;
	sf::Vector2f position;
	Direction direction;
	uint16_t hits;

	// Fight info
	FightMode fight_mode;
	civ::Ref<Ant> target;
	float dammage = 10.0f;
    float fight_dist = length * 0.25f;
    sf::Vector2f fight_pos;
    sf::Vector2f fight_vec;
    Cooldown attack_cooldown;
    bool enemy_found = false;
    float to_fight_timeout = 3.0f;
    float to_fight_time = 0.0f;
    
	Cooldown direction_update;
	Cooldown marker_add;
	Cooldown search_markers;
	float markers_count;
	float liberty_coef;
	float autonomy;
	float max_autonomy = 300.0f;

	uint32_t id;
	uint8_t col_id;

	Ant() = default;

	Ant(float x, float y, float angle, uint8_t colony_id)
		: position(x, y)
		, direction(angle)
		, direction_update(direction_update_period, RNGf::getUnder(1.0f) * direction_update_period)
		, marker_add(marker_period, RNGf::getUnder(1.0f) * marker_period)
		, search_markers(5.0f, 5.0f)
		, phase(Mode::ToFood)
		, liberty_coef(RNGf::getRange(0.001f, 0.01f))
		, hits(0)
        , fight_mode(FightMode::NoFight)
		, markers_count(0.0f)
		, autonomy(0.0f)
		, id(0)
		, col_id(colony_id)
        , attack_cooldown(1.5f, 0.0f)
	{
	}
    
    void addToWorldGrid(World& world)
    {
        ColonyCell& cell = world.map.get(position).markers[col_id];
        if (!cell.fighting) {
            cell.current_ant = id;
            cell.fighting = isFighting();
        }
    }
    
    bool isFighting() const
    {
        return fight_mode == FightMode::Fighting;
    }

	void update(float dt, World& world)
	{
		autonomy += dt;
        // Update current direction
        direction.update(dt);
        // Add ant to current cell
        addToWorldGrid(world);
        // Fight if needed
		if (isFighting()) {
			attack(dt);
            return;
        }
		updatePosition(world, dt);
        // If fight found, go for it
        if (fight_mode == FightMode::ToFight) {
            to_fight_time += dt;
            if (to_fight_time > to_fight_timeout) {
                fight_mode = FightMode::NoFight;
            }
            return;
        }
        if (autonomy > 0.75f * max_autonomy) {
            phase = Mode::Refill;
        }
        // Check collision with food
		if (phase == Mode::ToFood) {
			checkFood(world);
		}
		// Get current cell
		WorldCell& cell = world.map.get(position);
		cell.addPresence();
		search_markers.update(dt);
		direction_update.update(dt);
		if (direction_update.ready()) {
            direction_update.reset();
			if (search_markers.ready()) {
				findMarker(world);
                direction += RNGf::getFullRange(direction_noise_range);
			}
			else {
				cell.degrade(col_id, Mode::ToFood, 0.25f);
				direction += RNGf::getFullRange(2.0f * direction_noise_range);
			}
		}
		// Add marker
		marker_add.update(dt);
		if (marker_add.ready()) {
			addMarker(world);
			marker_add.reset();
		}
	}

	void attack(float dt)
	{
		if (target) {
            Ant& opponent = *target;
            position = fight_pos - fight_vec * (0.5f * length + (attack_cooldown.getRatio()) * fight_dist);
            attack_cooldown.update(dt);
            if (attack_cooldown.ready()) {
                attack_cooldown.reset();
                opponent.autonomy += dammage;
            }
		} else {
            enemy_found = true;
			fight_mode = FightMode::NoFight;
		}
	}

	void updatePosition(World& world, float dt)
	{
		sf::Vector2f v = direction.getVec();
		const sf::Vector2f next_position = position + (dt * move_speed) * v;
		const HitPoint hit = world.map.getFirstHit(position, v, dt * move_speed);
		if (hit.cell) {
			const uint32_t hits_threshold = 8;
			if (hits > hits_threshold) {
				const float new_angle = RNGf::getUnder(2.0f * PI);
				v = sf::Vector2f(cos(new_angle), sin(new_angle));
			}
			else {
				v.x *= hit.normal.x ? -1.0f : 1.0f;
				v.y *= hit.normal.y ? -1.0f : 1.0f;
			}
			++hits;
			direction.setDirectionNow(v);
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
		if (world.map.isOnFood(position)) {
			phase = Mode::ToHome;
			direction.addNow(PI);
			autonomy = 0.0f;
			markers_count = 0.0f;
			if (world.map.pickFood(position)) {
				phase = Mode::ToHomeNoFood;
				marker_add.target = repellent_period;
				marker_add.value = RNGf::getUnder(marker_add.target);
				// Add a repellent for 300s
				world.addMarkerRepellent(position, col_id, 300.0f);
			}
		}
	}

	void checkColony(ColonyBase& base)
	{
		if (getLength(position - base.position) < base.radius) {
			marker_add.target = marker_period;
			if (phase == Mode::ToHome || phase == Mode::ToHomeNoFood) {
				phase = Mode::ToFood;
				base.addFood(1.0f);
				direction.addNow(PI);
                base.enemies_found_count += enemy_found;
                enemy_found = false;
			}
			// Refill
			const float refill_cost = 1.0f;
			const float needed_refill = refill_cost * (autonomy / max_autonomy);
			autonomy = 0.0f;
			phase = Mode::ToFood;
			markers_count = 0.0f;
		}
	}

	Mode getMarkersSamplingType() const
	{
		if (phase == Mode::ToHome || phase == Mode::Refill || phase == Mode::ToHomeNoFood) {
			return Mode::ToHome;
		}
		return Mode::ToFood;
	}

	void findMarker(World& world)
	{
		// Init
		const Mode marker_phase = getMarkersSamplingType();
		const float sample_angle_range = PI * 0.5f;
		const float current_angle = direction.getCurrentAngle();
		float max_intensity = 0.0f;
		// To objective stuff
		sf::Vector2f max_direction = direction.getVec();
		WorldCell* max_cell = nullptr;
		bool found_permanent = false;
        bool found_fight = false;
		// Repellent stuff
		float max_repellent = 0.0f;
		WorldCell* repellent_cell = nullptr;
		// Sample the world
		const uint32_t sample_count = 32;
		for (uint32_t i(sample_count); i--;) {
			// Get random point in range
			const float sample_angle = current_angle + RNGf::getRange(sample_angle_range);
			const float distance = RNGf::getUnder(marker_detection_max_dist);
			const sf::Vector2f to_marker(cos(sample_angle), sin(sample_angle));
			auto* cell = world.map.getSafe(position + distance * to_marker);
			const HitPoint hit_result = world.map.getFirstHit(position, to_marker, distance);
			// Check cell
			if (!cell || hit_result.cell) {
				if (hit_result.cell) {
					hit_result.cell->discovered = 1.0f;
				}
				continue;
			}
            // Check if
            if (cell->checkEnemyPresence(col_id)) {
                found_fight = true;
                max_direction = to_marker;
                to_fight_time = 0.0f;
                break;
            }
			cell->discovered += 0.1f;
			// Check for food or colony
			if (cell->isPermanent(marker_phase, col_id) || (marker_phase == Mode::ToFood && cell->food)) {
				max_direction = to_marker;
				found_permanent = true;
				break;
			}
			// Flee if repellent
			if (cell->getRepellent(col_id) > max_repellent) {
				max_repellent = cell->getRepellent(col_id);
				repellent_cell = cell;
			}
			// Check for the most intense marker
			const float marker_intensity = to<float>(cell->getIntensity(marker_phase, col_id) * std::pow(cell->wall_dist, 2.0));
			if (marker_intensity > max_intensity) {
				max_intensity = marker_intensity;
				max_direction = to_marker;
				max_cell = cell;
			}
			// Eventually choose a different path
//            if (RNGf::proba(liberty_coef)) {
//			 	break;
//            }
		}
		// Check for repellent
		if (phase == Mode::ToFood && max_repellent && !found_permanent) {
			const float repellent_prob_factor = 0.3f;
			if (RNGf::proba(repellent_prob_factor * (1.0f - max_intensity / to<float>(Conf::MARKER_INTENSITY)))) {
				//phase = Mode::Flee;
				direction.addNow(RNGf::getUnder(2.0f * PI));
				search_markers.reset();
				return;
			}
		}
        if (found_fight) {
            direction = getAngle(max_direction);
            fight_mode = FightMode::ToFight;
            return;
        }
		// Remove repellent if still food
		if (repellent_cell && phase == Mode::ToHome) {
			repellent_cell->getRepellent(col_id) *= 0.95f;
		}
		// Update direction
		if (max_intensity) {
			if (RNGf::proba(0.2f) && phase == Mode::ToFood) {
				max_cell->degrade(col_id, phase, 0.99f);
			}
			direction = getAngle(max_direction);
		}
	}

	void addMarker(World& world)
	{
		markers_count += marker_add.target;
		if (phase == Mode::ToHome || phase == Mode::ToFood) {
			const float intensity = getMarkerIntensity(0.01f);
			world.addMarker(position, phase == Mode::ToFood ? Mode::ToHome : Mode::ToFood, intensity, col_id);
		}
		else if (phase == Mode::ToHomeNoFood) {
			const float intensity = to<float>(getMarkerIntensity(0.02f));
			world.addMarkerRepellent(position, col_id, intensity);
		}
	}

	void render_food(sf::VertexArray& va, const uint32_t index) const
	{
		constexpr float radius = 2.0f;
		sf::Vector2f food_pos(-10000.0f, -10000.0f);
		if (phase == Mode::ToHome || phase == Mode::ToHomeNoFood) {
			food_pos = position + length * 0.65f * direction.getVec();
		}

		va[index + 0].position = sf::Vector2f(food_pos.x - radius, food_pos.y - radius);
		va[index + 1].position = sf::Vector2f(food_pos.x + radius, food_pos.y - radius);
		va[index + 2].position = sf::Vector2f(food_pos.x + radius, food_pos.y + radius);
		va[index + 3].position = sf::Vector2f(food_pos.x - radius, food_pos.y + radius);
	}

	void render_in(sf::VertexArray& va, const uint32_t index) const
	{
        const float size_ratio = width / length;
		const sf::Vector2f dir_vec(direction.getVec() * length);
		const sf::Vector2f nrm_vec(-dir_vec.y * size_ratio, dir_vec.x * size_ratio);

		va[index + 0].position = position - nrm_vec + dir_vec;
		va[index + 1].position = position + nrm_vec + dir_vec;
		va[index + 2].position = position + nrm_vec - dir_vec;
		va[index + 3].position = position - nrm_vec - dir_vec;
	}

	float getMarkerIntensity(float coef)
	{
		return Conf::MARKER_INTENSITY * expf(-coef * markers_count);
	}

	void setTarget(civ::Ref<Ant> new_target)
	{
		target = new_target;
        fight_pos = 0.5f * (target->position + position);
        fight_vec = getNormalized(target->position - position);
		direction = getAngle(fight_vec);
	}
    
    void kill(World& world)
    {
        if (phase == Mode::ToHome || phase == Mode::ToHomeNoFood) {
            world.addFoodAt(position.x, position.y, 1);
        }
        world.addFoodAt(position.x, position.y, 5);
        phase = Mode::Dead;
    }
};
