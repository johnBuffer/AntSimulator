#pragma once
#include "async_va_renderer.hpp"
#include "common/grid.hpp"
#include "simulation/config.hpp"
#include "simulation/world/world_grid.hpp"
#include "common/utils.hpp"
#include <iostream>
#include "common/index_vector.hpp"


struct WorldRenderer : public AsyncRenderer
{
	const Grid<WorldCell>& grid;
	bool draw_markers       = false;
	bool draw_density       = false;
	bool draw_to_enemies    = false;
	int64_t selected_colony = -1;

    const float offset = 32.0f;
    const float intensity_factor = 255.0f / to<float>(Conf::MARKER_INTENSITY);
    const sf::Vector3f to_home_color = {to<float>(Conf::TO_HOME_COLOR.r) / 255.0f, to<float>(Conf::TO_HOME_COLOR.g) / 255.0f, to<float>(Conf::TO_HOME_COLOR.b) / 255.0f};
    const sf::Vector3f to_food_color = {to<float>(Conf::TO_FOOD_COLOR.r) / 255.0f, to<float>(Conf::TO_FOOD_COLOR.g) / 255.0f, to<float>(Conf::TO_FOOD_COLOR.b) / 255.0f};

    civ::Vector<sf::Color> colonies_color;

	WorldRenderer(Grid<WorldCell>& grid_, DoubleObject<sf::VertexArray>& target)
		: AsyncRenderer(target)
		, grid(grid_)
	{
		AsyncRenderer::start();
	}

	void initializeVertexArray(sf::VertexArray& va) override
	{
		va = sf::VertexArray(sf::Quads, grid.width * grid.height * 4);
		uint64_t i = 0;
		constexpr float eps = 0.01f;
		const auto cell_size = to<float>(grid.cell_size);
		const float cell_size_eps = cell_size + 2.0f * eps;
		for (int32_t y(0); y < grid.height; y++) {
			for (int32_t x(0); x < grid.width; x++) {
				const sf::Vector2f position(x * cell_size - eps, y * cell_size - eps);
				va[4 * i + 0].position = position;
				va[4 * i + 1].position = position + sf::Vector2f(cell_size_eps, 0.0f);
				va[4 * i + 2].position = position + sf::Vector2f(cell_size_eps, cell_size_eps);
				va[4 * i + 3].position = position + sf::Vector2f(0.0f         , cell_size_eps);
				++i;
			}
		}
	}

	void updateVertexArray() override
	{
		sf::VertexArray& va = vertex_array.getLast();

		uint64_t i = 0;
		for (int32_t y(0); y < grid.height; y++) {
			for (int32_t x(0); x < grid.width; x++) {
				const auto& cell = grid.getCst(sf::Vector2i(x, y));
				sf::Color color = sf::Color::Black;
				if (!cell.food && !cell.wall) {
                    if (draw_density) {
                        drawDensityOne(i, cell, va, color);
                    }
                    else if (selected_colony != -1) {
						const ColonyCell& colony_cell = cell.markers[selected_colony];
                        if (draw_to_enemies) {
                            drawToEnemy(i, colony_cell, va, color);
						}
                        else if (draw_markers) {
                            drawMarkers(i, colony_cell, va, color);
                        }
					}
				}
				else if (cell.food) {
					color = sf::Color(0, to<uint8_t>(std::min(255u, 100 + cell.food)), 0);
					const float food_offset = 1.0f;
					va[4 * i + 0].texCoords = sf::Vector2f(100.0f + food_offset, food_offset);
					va[4 * i + 1].texCoords = sf::Vector2f(200.0f - food_offset, food_offset);
					va[4 * i + 2].texCoords = sf::Vector2f(200.0f - food_offset, 100.0f - food_offset);
					va[4 * i + 3].texCoords = sf::Vector2f(100.0f + food_offset, 100.0f - food_offset);
				}
				else if (cell.wall) {
					const sf::Color base = Conf::WALL_COLOR;
					const float ratio = std::min(2.0f, 0.5f + cell.wall_dist);
					color = vec3ToColor(sf::Vector3f(base.r * ratio, base.g * ratio, base.b * ratio));
					va[4 * i + 0].texCoords = sf::Vector2f(200.0f + offset, offset);
					va[4 * i + 1].texCoords = sf::Vector2f(300.0f - offset, offset);
					va[4 * i + 2].texCoords = sf::Vector2f(300.0f - offset, 100.0f - offset);
					va[4 * i + 3].texCoords = sf::Vector2f(200.0f + offset, 100.0f - offset);
				}
				va[4 * i + 0].color = color;
				va[4 * i + 1].color = color;
				va[4 * i + 2].color = color;
				va[4 * i + 3].color = color;

				++i;
			}
		}
	}

    void drawToEnemy(uint64_t i, const ColonyCell& cell, sf::VertexArray& va, sf::Color& color) const
    {
        const sf::Vector3f to_enemies_color(255.0f, 0.0f, 255.0f);
        const sf::Vector3f intensity_color = intensity_factor * to_enemies_color * cell.intensity[2];
        color = vec3ToColor(intensity_color);
        va[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
        va[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
        va[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
        va[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
    }

    void drawMarkers(uint64_t i, const ColonyCell& cell, sf::VertexArray& va, sf::Color& color) const
    {
        if (cell.repellent != 0.0f) {
            color = sf::Color::Blue;
            va[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
            va[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
            va[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
            va[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
        }
        else {
            const sf::Vector3f intensity_1_color = intensity_factor * to_home_color * cell.intensity[0];
            const sf::Vector3f intensity_2_color = intensity_factor * to_food_color * cell.intensity[1];
            const sf::Vector3f mixed_color(
                    intensity_1_color.x + intensity_2_color.x,
                    intensity_1_color.y + intensity_2_color.y,
                    intensity_1_color.z + intensity_2_color.z
            );
            color = vec3ToColor(mixed_color);
            va[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
            va[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
            va[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
            va[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
        }
    }

    void drawDensityOne(uint64_t i, const WorldCell& cell, sf::VertexArray& va, sf::Color& color)
    {
        const sf::Color c_color = colonies_color[selected_colony];
        const float ratio = cell.density;
        color = vec3ToColor(sf::Vector3f{4.0f * ratio, ratio, ratio});
        va[4 * i + 0].texCoords = sf::Vector2f(200.0f + offset, offset);
        va[4 * i + 1].texCoords = sf::Vector2f(300.0f - offset, offset);
        va[4 * i + 2].texCoords = sf::Vector2f(300.0f - offset, 100.0f - offset);
        va[4 * i + 3].texCoords = sf::Vector2f(200.0f + offset, 100.0f - offset);
    }

    static sf::Color getDensityColor(sf::Vector3f density)
    {
        return sf::Color{};
    }
};
