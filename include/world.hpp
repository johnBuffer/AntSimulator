#pragma once
#include <list>
#include <vector>
#include <SFML/System.hpp>

#include "marker.hpp"
#include "food.hpp"
#include "utils.hpp"


template<typename T>
struct Grid
{
    Grid(int32_t width_, int32_t height_, uint32_t cell_size_)
        : cell_size(cell_size_)
        , width(width_ / cell_size_)
        , height(height_ / cell_size_)
    {
        cells.resize(width * height);
    }

    T* add(const T& obj)
    {
        return add(getCellCoords(obj.position), obj);
    }

    std::list<T>* getAt(const sf::Vector2f& position)
    {
        const sf::Vector2i cell_coords = getCellCoords(position);

        if (checkCell(cell_coords)) {
            return &cells[getIndexFromCoords(cell_coords)];
        }

        return nullptr;
    }

    std::list<T*> getAllAt(const sf::Vector2f& position)
    {
        std::list<T*> result(0);
        const sf::Vector2i cell_coords = getCellCoords(position);

        for (int32_t x(-1); x < 2; ++x) {
            for (int32_t y(-1); y < 2; ++y) {
                const sf::Vector2i coords = cell_coords + sf::Vector2i(x, y);
                if (checkCell(coords)) {
                    const uint64_t index = getIndexFromCoords(coords);
                    for (T& m : cells[index]) {
                        result.push_back(&m);
                    }
                }
            }
        }

        return result;
    }

    T* add(const sf::Vector2i& cell_coords, const T& obj)
    {
        if (checkCell(cell_coords)) {
            std::list<T>& l = cells[getIndexFromCoords(cell_coords)];
            if (Conf::MAX_MARKERS_PER_CELL > l.size()) {
                l.emplace_back(obj);
                return &l.back();
            }
        }
        return nullptr;
    }

    bool checkCell(const sf::Vector2i& cell_coords)
    {
        return cell_coords.x > -1 && cell_coords.x < width && cell_coords.y > -1 && cell_coords.y < height;
    }

    uint64_t getIndexFromCoords(const sf::Vector2i& cell_coords)
    {
        return cell_coords.x + cell_coords.y * width;
    }

    sf::Vector2i getCellCoords(const sf::Vector2f& position)
    {
        const int32_t x_cell = to<int32_t>(position.x / cell_size);
        const int32_t y_cell = to<int32_t>(position.y / cell_size);

        return sf::Vector2i(x_cell, y_cell);
    }

    std::vector<std::list<T>> cells;

    const int32_t width, height, cell_size;
};


struct World
{
    World(uint32_t width, uint32_t height)
        : grid_markers_home(width, height, 45)
        , grid_markers_food(width, height, 45)
        , grid_food(width, height, 5)
        , size(to<float>(width), to<float>(height))
        , va(sf::Quads)
    {}

    void removeExpiredMarkers()
    {
        // Home
        for (std::list<Marker>& l : grid_markers_home.cells) {
            l.remove_if([&](const Marker& m) {
                return m.isDone();
            });
        }
        // Food
        for (std::list<Marker>& l : grid_markers_food.cells) {
            l.remove_if([&](const Marker& m) {
                return m.isDone();
            });
        }
    }

    void removeExpiredFood()
    {
        for (std::list<Food>& l : grid_food.cells) {
            l.remove_if([&](const Food& m) {
                return m.isDone();
            });
        }
    }

    void update(const float dt)
    {
        removeExpiredMarkers();
        removeExpiredFood();

        markers_count = 0u;
        for (std::list<Marker>& l : grid_markers_home.cells) {
            for (Marker& m : l) {
                markers_count += m.permanent ? 0 : 1;
                m.update(dt);
            }
        }

        for (std::list<Marker>& l : grid_markers_food.cells) {
            for (Marker& m : l) {
                markers_count += m.permanent ? 0 : 1;
                m.update(dt);
            }
        }
    }

    Marker* addMarker(const Marker& marker)
    {
        return getGrid(marker.type).add(marker);
    }

    void render(sf::RenderTarget& target, const sf::RenderStates& states, bool draw_markers = true) const
    {
        if (draw_markers) {
            va.resize(4 * markers_count);
            generateMarkersVertexArray(va);
            sf::RenderStates rs = states;
            rs.texture = &(*Conf::MARKER_TEXTURE);
            target.draw(va, rs);
        }

        for (const std::list<Food>& l : grid_food.cells) {
            for (const Food& f : l) {
                f.render(target, states);
            }
        }
    }

    void generateMarkersVertexArray(sf::VertexArray& va) const
    {
        uint32_t current_index = 0;
        for (const std::list<Marker>& l : grid_markers_home.cells) {
            for (const Marker& m : l) {
                if (!m.permanent) {
                    m.render_in(va, 4 * (current_index++));
                }
            }
        }

        for (const std::list<Marker>& l : grid_markers_food.cells) {
            for (const Marker& m : l) {
                if (!m.permanent) {
                    m.render_in(va, 4 * (current_index++));
                }
            }
        }
    }

    void addFoodAt(float x, float y, float quantity)
    {
        Marker* marker = addMarker(Marker(sf::Vector2f(x, y), Marker::ToFood, 100000000.0f, true));
        if (marker) {
            grid_food.add(Food(x, y, 4.0f, quantity, marker));
        }
    }

    Grid<Marker>& getGrid(Marker::Type type)
    {
        if (type == Marker::ToFood) {
            return grid_markers_food;
        }
        return grid_markers_home;
    }

    sf::Vector2f size;
    mutable sf::VertexArray va;
    Grid<Marker> grid_markers_home;
    Grid<Marker> grid_markers_food;
    Grid<Food> grid_food;

    uint64_t markers_count;
};
