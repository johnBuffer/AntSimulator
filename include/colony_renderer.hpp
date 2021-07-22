#pragma once
#include <SFML/Graphics.hpp>
#include "config.hpp"
#include "colony.hpp"
#include "circular_gauge.hpp"


struct ColonyRenderer
{
	sf::Font font;
	sf::Text text;

	sf::VertexArray ants_va;
	sf::VertexArray ants_food_va;

	Graphic population;
	Graphic food_acc;
	Cooldown population_update;
	Colony& colony;

	ColonyRenderer(Colony& colony)
		: ants_va(sf::Quads, 4 * Conf::ANTS_COUNT)
		, ants_food_va(sf::Quads, 4 * Conf::ANTS_COUNT)
		, population(800, sf::Vector2f(800.0f, 100.0f), sf::Vector2f())
		, food_acc(800, sf::Vector2f(800.0f, 50.0f), sf::Vector2f())
		, population_update(3.0f)
		, colony(colony)
	{
		font.loadFromFile("res/font.ttf");
		text.setFont(font);

		food_acc.color = Conf::FOOD_COLOR;
		//food_acc.color.a = 200;

		for (uint64_t i(Conf::ANTS_COUNT-1); i--;) {
			const uint64_t index = 4 * i;
			// Ant
			ants_va[index + 0].color = colony.ants_color;
			ants_va[index + 1].color = colony.ants_color;
			ants_va[index + 2].color = colony.ants_color;
			ants_va[index + 3].color = colony.ants_color;
			ants_va[index + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
			ants_va[index + 1].texCoords = sf::Vector2f(73.0f, 0.0f);
			ants_va[index + 2].texCoords = sf::Vector2f(73.0f, 107.0f);
			ants_va[index + 3].texCoords = sf::Vector2f(0.0f, 107.0f);
			// Food
			ants_food_va[index + 0].color = Conf::FOOD_COLOR;
			ants_food_va[index + 1].color = Conf::FOOD_COLOR;
			ants_food_va[index + 2].color = Conf::FOOD_COLOR;
			ants_food_va[index + 3].color = Conf::FOOD_COLOR;
			ants_food_va[index + 0].texCoords = sf::Vector2f(100.0f, 0.0f);
			ants_food_va[index + 1].texCoords = sf::Vector2f(200.0f, 0.0f);
			ants_food_va[index + 2].texCoords = sf::Vector2f(200.0f, 100.0f);
			ants_food_va[index + 3].texCoords = sf::Vector2f(100.0f, 100.0f);
		}

		const float margin = 10.0f;
		const float GUI_MARGIN = 20.0f;
		population.x = GUI_MARGIN;
		population.y = Conf::WIN_HEIGHT - (population.height + GUI_MARGIN + 4.0f * margin) * colony.id;
		food_acc.x = GUI_MARGIN;
		food_acc.y = Conf::WIN_HEIGHT - population.height - GUI_MARGIN + 50.0f;

		population.color = colony.ants_color;
	}

	void renderAnts(sf::RenderTarget& target, const sf::RenderStates& states)
	{
		uint32_t index = 0;
		for (const Ant& a : colony.ants) {
			a.render_in(ants_va, 4 * index);
			a.render_food(ants_food_va, 4 * index);
			++index;
		}

		constexpr float no_draw_position = -10000.0f;
		const Ant placeholder(no_draw_position, no_draw_position, 0.0f, 0);
		for (uint32_t i(index); i < colony.max_ants_count; ++i) {
			placeholder.render_in(ants_va, 4 * i);
		}

		sf::RenderStates rs = states;
		rs.texture = &(*Conf::MARKER_TEXTURE);
		target.draw(ants_food_va, rs);
		rs.texture = &(*Conf::ANT_TEXTURE);
		target.draw(ants_va, rs);
	}

	void updatePopulation(float dt)
	{
		population.setLastValue(to<float>(colony.ants.size()));
		food_acc.setLastValue(colony.base.food_acc_mean.get());

		population_update.update(dt);
		if (population_update.ready()) {
			population_update.reset();
			population.next();
			food_acc.next();
		}
	}

	void render(sf::RenderTarget& target, const sf::RenderStates& states)
	{
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

		const float margin = 10.0f;
		sf::RectangleShape population_background(sf::Vector2f(population.width + 2.0f * margin,
															  population.height + 5.0f * margin));
		population_background.setFillColor(sf::Color(50, 50, 50, 200));
		population_background.setPosition(sf::Vector2f(population.x, population.y) - sf::Vector2f(margin, 4.0f * margin));
		target.draw(population_background);

		const uint32_t ants_count = to<int32_t>(colony.ants.size());
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);
		text.setPosition(population.x, population.y - 4.0f * margin);
		text.setString("Population " + toStr(ants_count));
		target.draw(text);

		text.setCharacterSize(14);
		const int64_t pop_diff = colony.pop_diff.get();
		text.setFillColor(pop_diff >= 0 ? sf::Color::Green : sf::Color::Red);
		text.setPosition(population.x + 160.0f, population.y - 3.5f * margin);
		const std::string sign = (pop_diff >= 0 ? "+" : "");
		text.setString("(" + sign + toStr(pop_diff) + " Ants over last 60s)");
		target.draw(text);

		text.setCharacterSize(14);
		text.setFillColor(Conf::FOOD_COLOR);
		text.setPosition(population.x, population.y - 1.8f * margin);
		text.setString("+ " + toStr(int(10.0f * colony.base.food_acc_mean.get())) + " Food/s");
		target.draw(text);

		population.render(target);
	}
};
