#pragma once
#include "async_va_renderer.hpp"
#include "grid.hpp"
#include "config.hpp"
#include "world_grid.hpp"
#include "utils.hpp"


struct WorldRenderer : public AsyncRenderer
{
	const Grid<WorldCell>& grid;
	bool draw_markers;
	bool draw_density;

	WorldRenderer(Grid<WorldCell>& grid_, DoubleObject<sf::VertexArray>& target)
		: AsyncRenderer(target)
		, grid(grid_)
		, draw_markers(true)
		, draw_density(false)
	{
		AsyncRenderer::start();
	}

	void initializeVertexArray(sf::VertexArray& va) override
	{
		va = sf::VertexArray(sf::Quads, grid.width * grid.height * 4);
		uint64_t i = 0;
		constexpr float eps = 0.01f;
		const float cell_size = to<float>(grid.cell_size);
		const float cell_size_eps = cell_size + 2.0f * eps;
		for (int32_t x(0); x < grid.width; x++) {
			for (int32_t y(0); y < grid.height; y++) {
				const sf::Vector2f position(x * cell_size - eps, y * cell_size - eps);
				va[4 * i + 0].position = position;
				va[4 * i + 1].position = position + sf::Vector2f(cell_size_eps, 0.0f);
				va[4 * i + 2].position = position + sf::Vector2f(cell_size_eps, cell_size_eps);
				va[4 * i + 3].position = position + sf::Vector2f(0.0f, cell_size_eps);
				++i;
			}
		}
	}

	void updateVertexArray() override
	{
		sf::VertexArray& va = vertex_array.getLast();
		const sf::Vector3f to_home_color(Conf::TO_HOME_COLOR.r / 255.0f, Conf::TO_HOME_COLOR.g / 255.0f, Conf::TO_HOME_COLOR.b / 255.0f);
		const sf::Vector3f to_food_color(Conf::TO_FOOD_COLOR.r / 255.0f, Conf::TO_FOOD_COLOR.g / 255.0f, Conf::TO_FOOD_COLOR.b / 255.0f);
		const float intensity_factor = 2.0f * 255.0f / to<float>(Conf::MARKER_INTENSITY);

		uint64_t i = 0;
		for (int32_t x(0); x < grid.width; x++) {
			for (int32_t y(0); y < grid.height; y++) {
				const auto& cell = grid.getCst(sf::Vector2i(x, y));
				sf::Color color = sf::Color::Black;
				if (!cell.food && !cell.wall) {
					if (draw_density) {
						/*color = vec3ToColor(sf::Vector3f(255.0f, 73.0f, 68.0f) * cell.wall_dist);
						const float offset = 4.0f;
						va[4 * i + 0].texCoords = sf::Vector2f(200.0f + offset, offset);
						va[4 * i + 1].texCoords = sf::Vector2f(300.0f - offset, offset);
						va[4 * i + 2].texCoords = sf::Vector2f(300.0f - offset, 100.0f - offset);
						va[4 * i + 3].texCoords = sf::Vector2f(200.0f + offset, 100.0f - offset);*/
					}
					else {
						/*if ((cell.intensity[0] < 1.0 && cell.intensity[1] < 1.0) || !draw_markers) {
							const float ratio = std::min(1.0f, cell.discovered);
							color = sf::Color(50 * ratio, 50 * ratio, 50 * ratio);
							const float offset = 32.0f;
							va[4 * i + 0].texCoords = sf::Vector2f(200.0f + offset, offset);
							va[4 * i + 1].texCoords = sf::Vector2f(300.0f - offset, offset);
							va[4 * i + 2].texCoords = sf::Vector2f(300.0f - offset, 100.0f - offset);
							va[4 * i + 3].texCoords = sf::Vector2f(200.0f + offset, 100.0f - offset);
						}
						else if (draw_markers) {
							const float offset = 32.0f;
							if (cell.repellent) {
								color = sf::Color::Blue;
								va[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
								va[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
								va[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
								va[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
							}
							else {
								const sf::Vector3f intensity_1_color = intensity_factor * to_home_color * float(cell.intensity[0]);
								const sf::Vector3f intensity_2_color = intensity_factor * to_food_color * float(cell.intensity[1]);
								const sf::Vector3f mixed_color(
									std::min(255.0f, intensity_1_color.x + intensity_2_color.x),
									std::min(255.0f, intensity_1_color.y + intensity_2_color.y),
									std::min(255.0f, intensity_1_color.z + intensity_2_color.z)
								);
								color = sf::Color(to<uint8_t>(mixed_color.x), to<uint8_t>(mixed_color.y), to<uint8_t>(mixed_color.z));
								va[4 * i + 0].texCoords = sf::Vector2f(offset, offset);
								va[4 * i + 1].texCoords = sf::Vector2f(100.0f - offset, offset);
								va[4 * i + 2].texCoords = sf::Vector2f(100.0f - offset, 100.0f - offset);
								va[4 * i + 3].texCoords = sf::Vector2f(offset, 100.0f - offset);
							}
						}*/
					}
				}
				else if (cell.food) {
					const float max_food = 20.0f;
					const float ratio = cell.food / max_food * (cell.discovered > 0.0f ? 1.0f : 0.0f);
					color = sf::Color(0, to<uint8_t>(ratio * 255), 0);
					const float offset = 4.0f;
					va[4 * i + 0].texCoords = sf::Vector2f(100.0f + offset, offset);
					va[4 * i + 1].texCoords = sf::Vector2f(200.0f - offset, offset);
					va[4 * i + 2].texCoords = sf::Vector2f(200.0f - offset, 100.0f - offset);
					va[4 * i + 3].texCoords = sf::Vector2f(100.0f + offset, 100.0f - offset);
				}
				else if (cell.wall) {
					const sf::Color base = Conf::WALL_COLOR;
					const float ratio = std::min(1.0f, cell.discovered);
					color = vec3ToColor(sf::Vector3f(base.r * ratio, base.g * ratio, base.b * ratio));
					const float offset = 10.0f;
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
};
