#pragma once

#include <SFML/Graphics.hpp>
#include "world.hpp"
#include "colony.hpp"


class DisplayManager
{
public:
    DisplayManager(sf::RenderTarget& target, sf::RenderWindow& window, World& world, Colony& colony);

    //offset mutators
    void setOffset(float x, float y) {m_offsetX=x; m_offsetY=y;};
    void setOffset(const sf::Vector2f& off) {m_offsetX=off.x; m_offsetY=off.y;};

    void addOffset(float x, float y) {m_offsetX-=x/m_zoom; m_offsetY-=y/m_zoom;};
    void addOffset(const sf::Vector2f& off) {m_offsetX-=off.x/m_zoom; m_offsetY-=off.y/m_zoom;};

    // set the absolute zoom
    void setZoom(float zoom) {m_zoom = zoom;};

    // zoom
    void zoom(float zoomFactor) {m_zoom *= zoomFactor;};

    // draw the current world
    void draw();

	void processEvents();

    // getters
    sf::Vector2f getOffset() const {return sf::Vector2f(m_offsetX, m_offsetY);};
    float getZoom() const {return m_zoom;};
	sf::Vector2f worldCoordToDisplayCoord(const sf::Vector2f&);
	sf::Vector2f displayCoordToWorldCoord(const sf::Vector2f&);

	bool clic;
	bool pause;
	bool update;
	float render_time;
	bool speed_mode;
	bool debug_mode;
	bool wall_mode;
	bool render_ants;
	bool remove_wall;

	sf::Vector2f getClicPosition() const
	{
		return sf::Vector2f(to<float>(m_clic_position.x), to<float>(m_clic_position.y));
	}

private:
	sf::RenderTarget& m_target;
	sf::RenderWindow& m_window;
	sf::VertexArray m_va;

	World& m_world;
	Colony& m_colony;

	bool m_mouse_button_pressed;
	sf::Vector2i m_drag_clic_position, m_clic_position;

    float m_zoom, m_offsetX, m_offsetY, m_windowOffsetX, m_windowOffsetY;
};
