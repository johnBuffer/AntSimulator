#include "display_manager.hpp"


DisplayManager::DisplayManager(sf::RenderTarget& target, sf::RenderWindow& window, World& world, Colony& colony)
	: m_window(window)
	, m_target(target)
	, m_zoom(1.0f)
	, m_offsetX(0.0f)
	, m_offsetY(0.0f)
	, speed_mode(false)
	, m_va(sf::Quads, 0)
	, update(true)
	, debug_mode(false)
	, m_world(world)
	, m_colony(colony)
	, clic(false)
	, m_mouse_button_pressed(false)
	, pause(false)
	, draw_markers(true)
	, wall_mode(false)
{
	m_windowOffsetX = m_window.getSize().x * 0.5f;
    m_windowOffsetY = m_window.getSize().y * 0.5f;

	m_offsetX = m_windowOffsetX;
	m_offsetY = m_windowOffsetY;

    m_texture.loadFromFile("res/circle.png");
}

sf::Vector2f DisplayManager::worldCoordToDisplayCoord(const sf::Vector2f& worldCoord)
{
    float worldX = worldCoord.x;
    float worldY = worldCoord.y;

    float viewCoordX = (worldX-m_offsetX)*m_zoom+m_windowOffsetX;
    float viewCoordY = (worldY-m_offsetY)*m_zoom+m_windowOffsetY;

    return sf::Vector2f(viewCoordX, viewCoordY);
}

sf::Vector2f DisplayManager::displayCoordToWorldCoord(const sf::Vector2f& viewCoord)
{
    float viewCoordX = viewCoord.x;
    float viewCoordY = viewCoord.y;

    float worldCoordX = (viewCoordX-m_windowOffsetX)/m_zoom+m_offsetX;
    float worldCoordY = (viewCoordY-m_windowOffsetY)/m_zoom+m_offsetY;

    return sf::Vector2f(worldCoordX, worldCoordY);
}

void DisplayManager::draw()
{
	sf::Clock clock;
    // draw the world's ground as a big black square
    sf::RectangleShape ground(sf::Vector2f(m_world.size.x, m_world.size.y));
    ground.setFillColor(sf::Color::Black);

	sf::RenderStates rs_ground;
	rs_ground.transform.translate(m_windowOffsetX, m_windowOffsetY);
	rs_ground.transform.scale(m_zoom, m_zoom);
	rs_ground.transform.translate(-m_offsetX, -m_offsetY);
    m_target.draw(ground, rs_ground);

	sf::RenderStates rs;
	//rs.texture = &m_texture;
	rs.transform.translate(m_windowOffsetX, m_windowOffsetY);
	rs.transform.scale(m_zoom, m_zoom);
	rs.transform.translate(-m_offsetX, -m_offsetY);

	m_world.render(m_target, rs, draw_markers);
	m_colony.render(m_target, rs);

	render_time = clock.getElapsedTime().asMicroseconds() * 0.001f;
}


void DisplayManager::processEvents()
{
	sf::Vector2i mousePosition = sf::Mouse::getPosition(m_window);

	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			if (event.key.code == sf::Keyboard::Escape) m_window.close();
			else if ((event.key.code == sf::Keyboard::Subtract)) zoom(0.8f);
			else if ((event.key.code == sf::Keyboard::Add)) zoom(1.2f);
			else if ((event.key.code == sf::Keyboard::Space)) update = !update;
			else if ((event.key.code == sf::Keyboard::E)) pause = !pause;
			else if ((event.key.code == sf::Keyboard::A)) draw_markers = !draw_markers;
			else if ((event.key.code == sf::Keyboard::D)) debug_mode = !debug_mode;
			else if ((event.key.code == sf::Keyboard::W)) wall_mode = !wall_mode;
			else if ((event.key.code == sf::Keyboard::R))
			{
				m_offsetX = 0.0f;
				m_offsetY = 0.0f;
				m_zoom = 1.0f;
			}
			else if ((event.key.code == sf::Keyboard::S))
			{
				speed_mode = !speed_mode;
				if (speed_mode)
					m_window.setFramerateLimit(0);
				else
					m_window.setFramerateLimit(60);
			}
			break;
		case sf::Event::MouseWheelMoved:
			// this is an amazing zoom
			zoom(1 + event.mouseWheel.delta * 0.2f);
			break;
		case sf::Event::MouseButtonPressed:
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				m_mouse_button_pressed = true;
				m_drag_clic_position = mousePosition;
				m_clic_position = mousePosition;
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				clic = true;
			}
			break;
		case sf::Event::MouseButtonReleased:
			m_mouse_button_pressed = false;
			if (event.mouseButton.button == sf::Mouse::Right)
			{
				clic = false;
			}
			break;
		case sf::Event::MouseMoved:
			if (m_mouse_button_pressed) // in this case we are dragging
			{
				// updating displayManager offset
				const float vx = float(mousePosition.x - m_drag_clic_position.x);
				const float vy = float(mousePosition.y - m_drag_clic_position.y);
				addOffset(vx, vy);
				m_drag_clic_position = mousePosition;
			}
			break;
		default:
			break;
		}
	}
}

