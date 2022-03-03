#pragma once

#include <list>
#include "simulation/world/world.hpp"
#include "simulation/config.hpp"
#include "common/direction.hpp"
#include "common/number_generator.hpp"
#include "ant_mode.hpp"
#include "common/cooldown.hpp"
#include "simulation/colony/colony_base.hpp"
#include "common/index_vector.hpp"


struct SamplingResult
{
	float max_intensity         = 0.0f;
	// To objective stuff
	sf::Vector2f max_direction  = {0.0f, 0.0f};
	WorldCell* max_cell         = nullptr;
	bool found_permanent        = false;
	bool found_fight            = false;
	// Repellent stuff
	float max_repellent         = 0.0f;
	WorldCell* repellent_cell   = nullptr;
};


struct Ant
{
	enum class Type
	{
		Worker,
		Soldier
	};

	// Parameters
	static constexpr float move_speed                = 40.0f;
	static constexpr float marker_detection_max_dist = 40.0f;
	static constexpr float direction_update_period   = 0.25f;
	static constexpr float marker_period             = 0.25f;
	static constexpr float direction_noise_range     = PI * 0.02f;
	static constexpr float repellent_period          = 128.0f;

    float width           = 3.0f;
    float length          = 4.7f;
	Mode phase            = Mode::ToFood;
    uint16_t hits         = 0;
	sf::Vector2f position;
	Direction direction;

	// Fight info
	FightMode fight_mode    = FightMode::NoFight;
	float damage            = 10.0f;
    float fight_dist        = length * 0.25f;
    civ::Ref<Ant> target;
    sf::Vector2f fight_pos;
    sf::Vector2f fight_vec;
    bool enemy_found        = false;
	float enemy_intensity   = 0.0f;
    float to_fight_timeout  = 1.0f;
    float to_fight_time     = 0.0f;
	AntRef fight_request;

    Cooldown attack_cooldown;
    Cooldown direction_update;
	Cooldown marker_add;
	Cooldown search_markers;
	float internal_clock         = 0.0f;
	float to_enemy_markers_count = 0.0f;
    float max_autonomy           = 300.0f;
    float liberty_coef           = 0.0f;
	float autonomy               = 0.0f;

	int16_t id     = 0;
	uint8_t col_id = 0;
	Type type      = Type::Worker;

	Ant() = default;
	Ant(float x, float y, float angle, uint8_t colony_id)
		: position(x, y)
		, direction(angle)
		, direction_update(direction_update_period, RNGf::getUnder(1.0f) * direction_update_period)
		, marker_add(marker_period, RNGf::getUnder(1.0f) * marker_period)
		, search_markers(5.0f, 5.0f)
		, phase(Mode::ToFood)
		, liberty_coef(RNGf::getRange(0.001f, 0.01f))
        , fight_mode(FightMode::NoFight)
		, col_id(colony_id)
        , attack_cooldown(1.5f, 0.0f)
		, type(Type::Worker)
	{
	}
    
    void addToWorldGrid(World& world)
    {
		WorldCell* cell = world.map.getSafe(position);
		if (!cell) {
			// Set as dead if outside the world
			terminate();
			return;
		}

		ColonyCell& colony_cell = cell->markers[col_id];
        if (!colony_cell.fighting) {
			colony_cell.current_ant = id;
			colony_cell.fighting = isFighting();
        }
    }

	void removeFromWorldGrid(World& world) const
	{
		ColonyCell& cell = world.map.get(position).markers[col_id];
		if (cell.current_ant == id) {
			cell.current_ant = -1;
		}
	}

    [[nodiscard]]
    bool isFighting() const
    {
        return fight_mode == FightMode::Fighting;
    }

	void attack(float dt)
	{
		if (target) {
            Ant& opponent = *target;
            position = fight_pos - fight_vec * (0.5f * length + (attack_cooldown.getRatio()) * fight_dist);
            attack_cooldown.update(dt);
            if (attack_cooldown.ready()) {
                attack_cooldown.reset();
                opponent.autonomy += damage;
            }
		} else {
			fight_mode = FightMode::NoFight;
			if (type == Type::Soldier) {
				// Restore some energy
				autonomy = std::max(0.0f, autonomy - 3.0f);
			}
		}
	}

	void updatePosition(World& world, float dt)
	{
		sf::Vector2f v = direction.getVec();
		const HitPoint hit = world.map.getFirstHit(position, v, dt * move_speed);
		if (hit.cell) {
			const uint32_t hits_threshold = 4;
			if (hits > hits_threshold) {
				terminate();
			}
			else {
				v.x *= hit.normal.x != 0.0f ? -1.0f : 1.0f;
				v.y *= hit.normal.y != 0.0f ? -1.0f : 1.0f;
			}
			++hits;
			direction.setDirectionNow(v);
		}
		else {
			hits = 0;
			position += (dt * move_speed) * v;
			// Ants outside the map go back to home
			if (position.x < 0.0f || position.x > to<float>(Conf::WORLD_WIDTH) ||
			    position.y < 0.0f || position.y > to<float>(Conf::WORLD_HEIGHT)) {
				terminate();
			}
		}
	}

	void checkFood(World& world)
	{
		if (world.map.isOnFood(position)) {
			phase = Mode::ToHome;
			direction.addNow(PI);
			autonomy = 0.0f;
			internal_clock = 0.0f;
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
				base.addFood(1.0f);
				direction.addNow(PI);
                base.enemies_found_count += enemy_found;
			}
			// Refill
			if (!isFighting()) {
				autonomy = 0.0f;
			}
			enemy_intensity = 0.0f;
			resetMarkers();
			enemy_found = false;
			if (type == Type::Soldier) {
				phase = Mode::ToEnemy;
			}
			else {
				phase = Mode::ToFood;
			}
		}
	}

	void updateClocks(float dt)
	{
		autonomy += dt;
		internal_clock += dt;
		to_enemy_markers_count += dt;
	}

    [[nodiscard]]
	Mode getMarkersSamplingType() const
	{
		if (phase == Mode::ToHome || phase == Mode::Refill || phase == Mode::ToHomeNoFood) {
			return Mode::ToHome;
		}
		return phase;
	}

	void resetMarkers()
	{
		internal_clock = 0.0f;
		to_enemy_markers_count = 0.0f;
	}

	void addMarker(World& world) const
	{
		if (phase == Mode::ToHome || phase == Mode::ToFood) {
			const float intensity = getMarkerIntensity(0.05f, internal_clock);
			world.addMarker(position, phase == Mode::ToFood ? Mode::ToHome : Mode::ToFood, intensity, col_id);
		}
		else if (phase == Mode::ToHomeNoFood) {
			const auto intensity = to<float>(getMarkerIntensity(0.1f, internal_clock));
			world.addMarkerRepellent(position, col_id, intensity);
		}
		if (enemy_found) {
			// If enemy found add ToEnemy markers
			const float intensity = std::min(0.1f, enemy_intensity) * getMarkerIntensity(0.05f, to_enemy_markers_count);
			world.addMarker(position, Mode::ToEnemy, intensity, col_id);
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

    [[nodiscard]]
	static float getMarkerIntensity(float coef, float count)
	{
		return Conf::MARKER_INTENSITY * expf(-coef * count);
	}

	void setTarget(civ::Ref<Ant> new_target)
	{
		// Disable fight request
		fight_request.active = false;
		// Set fight parameters
		fight_mode = FightMode::Fighting;
		target = new_target;
        fight_pos = 0.5f * (target->position + position);
        fight_vec = getNormalized(target->position - position);
		direction = getAngle(fight_vec);
		enemy_found = true;
	}
    
    void kill(World& world)
    {
        if (phase == Mode::ToHome || phase == Mode::ToHomeNoFood) {
            world.addFoodAt(position.x, position.y, 1);
        }
        //world.addFoodAt(position.x, position.y, 2);
        phase = Mode::Dead;
		removeFromWorldGrid(world);
    }

	void detectEnemy()
	{
		enemy_found = true;
		to_enemy_markers_count = 0.0f;
		to_fight_time = 0.0f;
		enemy_intensity += 0.001f;
	}

	void request_fight(AntRef ref)
	{
		fight_mode = FightMode::ToFight;
		fight_request = ref;
	}

	void terminate()
	{
		autonomy = max_autonomy + 1.0f;
	}

    [[nodiscard]]
	bool isDone() const
	{
		return autonomy >= max_autonomy;
	}

    [[nodiscard]]
	bool isDead() const
	{
		return phase == Mode::Dead;
	}
};
