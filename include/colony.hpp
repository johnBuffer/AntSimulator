#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <list>
#include "ant.hpp"
#include "utils.hpp"
#include "world.hpp"


struct Colony
{
    Colony(float x, float y, uint32_t n)
        : position(x, y)
        , last_direction_update(0.0f)
        , ants_va(sf::Quads, 4 * n)
    {
        for (uint32_t i(n); i--;) {
            ants.emplace_back(x, y, getRandRange(2.0f * PI), n - i - 1);
        }

        for (uint64_t i(0); i < n; ++i) {
            const uint64_t index = 4 * i;
            ants_va[index + 0].color = Conf::ANT_COLOR;
            ants_va[index + 1].color = Conf::ANT_COLOR;
            ants_va[index + 2].color = Conf::ANT_COLOR;
            ants_va[index + 3].color = Conf::ANT_COLOR;

            ants_va[index + 0].texCoords = sf::Vector2f(0.0f, 0.0f);
            ants_va[index + 1].texCoords = sf::Vector2f(73.0f, 0.0f);
            ants_va[index + 2].texCoords = sf::Vector2f(73.0f, 107.0f);
            ants_va[index + 3].texCoords = sf::Vector2f(0.0f, 107.0f);
        }
    }

    void update(const float dt, World& world)
    {
        for (Ant& ant : ants) {
            ant.update(dt, world);
        }

        for (Ant& ant : ants) {
            ant.checkColony(position);
        }
    }

    void render(sf::RenderTarget& target, const sf::RenderStates& states) const
    {
        for (const Ant& a : ants) {
            a.render_food(target, states);
        }

        uint32_t index = 0;
        for (const Ant& a : ants) {
            a.render_in(ants_va, 4 * (index++));
        }

        sf::RenderStates rs = states;
        rs.texture = &(*Conf::ANT_TEXTURE);
        target.draw(ants_va, rs);

        sf::CircleShape circle(size);
        circle.setOrigin(size, size);
        circle.setPosition(position);
        circle.setFillColor(Conf::COLONY_COLOR);
        target.draw(circle, states);
    }

    const sf::Vector2f position;
    std::vector<Ant> ants;
    mutable sf::VertexArray ants_va;
    const float size = 20.0f;

    float last_direction_update;
    const float direction_update_period = 0.25f;

};
