#pragma once
#include <SFML/Graphics.hpp>
#include "simulation/config.hpp"
#include "simulation/colony/colony.hpp"
#include "common/circular_gauge.hpp"


struct PopulationChart
{
	float padding = 10.0f;
	
	sf::Vector2f position;
	sf::Vector2f size;

	Cooldown population_update;
	Graphic population;
	
	sf::Font font;
	sf::Text text;

	uint32_t ants_count = 0;
	uint32_t soldiers_count = 0;
	int32_t pop_diff = 0;

	PopulationChart()
		: population(800, sf::Vector2f(800.0f, 100.0f), sf::Vector2f())
		, population_update(3.0f)
	{
		font.loadFromFile("res/font.ttf");
		text.setFont(font);
	}

	void configure(sf::Vector2f pos, sf::Vector2f size_)
	{
		size = size_;
		position = pos;

		population.width  = size.x - 2.0f * padding;
		population.height = size.y - 4.0f * padding;

		population.x = position.x + padding;
		population.y = position.y + 3.0f * padding;
	}

	void updateData(Colony& colony, float dt)
	{
		pop_diff = to<int32_t>(colony.pop_diff.get());
		ants_count = to<int32_t>(colony.ants.size());
		soldiers_count = to<int32_t>(colony.soldiersCount());
		population.setLastValue(to<float>(ants_count));

		population_update.update(dt);
		if (population_update.ready()) {
			population_update.reset();
			population.next();
		}
	}

	void render(sf::RenderTarget& target)
	{
		sf::RectangleShape population_background(size);
		population_background.setFillColor(sf::Color(50, 50, 50, 200));
		population_background.setPosition(position);
		target.draw(population_background);

		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);
		text.setPosition(position.x + padding, position.y);
		text.setString("Population " + toStr(ants_count) + "/" + toStr(soldiers_count));
		target.draw(text);

		text.setCharacterSize(14);
		text.setFillColor(pop_diff >= 0 ? sf::Color::Green : sf::Color::Red);
		//text.setPosition(position.x + 200.0f, position.y + 0.5f * padding);
		const std::string sign = (pop_diff >= 0 ? "+" : "");
		text.setString("(" + sign + toStr(pop_diff) + " last 60s)");
		// right justify
		sf::FloatRect bounds = text.getLocalBounds();
		text.setPosition((position.x + population.width + padding) - bounds.width, position.y + 0.5f * padding);

		target.draw(text);

		//population.render(target);
	}
};


struct ColonyRenderer
{
	sf::Font         font;
	sf::Text         text;
	sf::VertexArray  ants_va;
	sf::VertexArray  ants_food_va;
	PopulationChart  population;
	civ::Ref<Colony> colony_ref;

    ColonyRenderer() = default;

    explicit
	ColonyRenderer(civ::Ref<Colony> colony)
		: ants_va(sf::Quads, 4 * Conf::ANTS_COUNT)
		, ants_food_va(sf::Quads, 4 * Conf::ANTS_COUNT)
		, colony_ref(colony)
	{
		font.loadFromFile("res/font.ttf");
		text.setFont(font);

        initializeAntsVA();
		for (uint64_t i(Conf::ANTS_COUNT-1); i--;) {
			const uint64_t index = 4 * i;
			// Food
			ants_food_va[index + 0].color = Conf::FOOD_COLOR;
			ants_food_va[index + 1].color = Conf::FOOD_COLOR;
			ants_food_va[index + 2].color = Conf::FOOD_COLOR;
			ants_food_va[index + 3].color = Conf::FOOD_COLOR;
			ants_food_va[index + 0].texCoords = sf::Vector2f(100.0f, 0.0f);
			ants_food_va[index + 1].texCoords = sf::Vector2f(199.0f, 0.0f);
			ants_food_va[index + 2].texCoords = sf::Vector2f(199.0f, 100.0f);
			ants_food_va[index + 3].texCoords = sf::Vector2f(100.0f, 100.0f);
            ants_food_va[index + 0].position = sf::Vector2f(0.0f, 0.0f);
            ants_food_va[index + 1].position = sf::Vector2f(0.0f, 0.0f);
            ants_food_va[index + 2].position = sf::Vector2f(0.0f, 0.0f);
            ants_food_va[index + 3].position = sf::Vector2f(0.0f, 0.0f);
		}
	
		const float margin = 10.0f;
		const sf::Vector2f size(400.0f, 100.0f);
		const float colonies_count = 2.0f;
		const float start_x = (Conf::WIN_WIDTH - size.x * colonies_count - (colonies_count - 1.0f) * margin) * 0.5f;
		population.configure({start_x + (size.x + margin) * colony_ref->id, margin}, size);
		population.population.color = colony_ref->ants_color;
	}

    void initializeAntsVA()
    {
        const Colony& colony = *colony_ref;
        for (uint64_t i(Conf::ANTS_COUNT-1); i--;) {
            const uint64_t index = 4 * i;
            // Ant
            ants_va[index + 0].color = colony.ants_color;
            ants_va[index + 1].color = colony.ants_color;
            ants_va[index + 2].color = colony.ants_color;
            ants_va[index + 3].color = colony.ants_color;
            ants_va[index + 0].texCoords = sf::Vector2f(0.0f  , 0.0f);
            ants_va[index + 1].texCoords = sf::Vector2f(370.0f, 0.0f);
            ants_va[index + 2].texCoords = sf::Vector2f(370.0f, 552.0f);
            ants_va[index + 3].texCoords = sf::Vector2f(0.0f  , 552.0f);
            ants_va[index + 0].position = sf::Vector2f(0.0f   , 0.0f);
            ants_va[index + 1].position = sf::Vector2f(0.0f   , 0.0f);
            ants_va[index + 2].position = sf::Vector2f(0.0f   , 0.0f);
            ants_va[index + 3].position = sf::Vector2f(0.0f   , 0.0f);
        }
    }

	void renderAnts(sf::RenderTarget& target, const sf::RenderStates& states)
	{
        Colony& colony = *colony_ref;
        if (colony.color_changed) {
            initializeAntsVA();
            colony.color_changed = false;
        }
		uint32_t index = 0;
		for (const Ant& a : colony.ants) {
			a.render_in(ants_va, 4 * index);
			a.render_food(ants_food_va, 4 * index);
			++index;
		}

		sf::RenderStates rs = states;
		rs.texture = &(*Conf::MARKER_TEXTURE);
		target.draw(ants_food_va, rs);
		rs.texture = &(*Conf::ANT_TEXTURE);
		target.draw(ants_va, rs);
	}
    
    void cleanVAs(uint32_t from, uint32_t to)
    {
        for (uint32_t i(from); i<to; ++i) {
            ants_va[4 * i + 0].position = {};
            ants_va[4 * i + 1].position = {};
            ants_va[4 * i + 2].position = {};
            ants_va[4 * i + 3].position = {};
            ants_food_va[4 * i + 0].position = {};
            ants_food_va[4 * i + 1].position = {};
            ants_food_va[4 * i + 2].position = {};
            ants_food_va[4 * i + 3].position = {};
        }
    }

	void updatePopulation(float dt)
	{
		population.updateData(*colony_ref, dt);
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states)
	{
        Colony& colony = *colony_ref;

        const float size = colony.base.radius;
		sf::CircleShape circle(size);
		circle.setOrigin(size, size);
		circle.setPosition(colony.base.position);
		circle.setFillColor(colony.ants_color);
		target.draw(circle, states);

		CircularGauge food_gauge(colony.base.position, 12.0f, 17.0f, sf::Color::White);
		food_gauge.max_value = colony.base.max_food;
		food_gauge.current_value = colony.base.food;
		food_gauge.render(target, states);
	}

	void render_charts(sf::RenderTarget& target)
	{
		population.render(target);
	}
};
