#include <SFML/Graphics.hpp>


float getRandRange(float width)
{
	constexpr int32_t max_value = 10000;
	constexpr int32_t half = max_value / 2;
	constexpr float div = 1.0f / float(max_value);
	const float val = (rand() % max_value - half) * div * width;
	return val;
}


struct Ant
{
	Ant() = default;

	Ant(float x, float y, float angle)
		: position(x, y)
		, direction(angle)
		, last_direction_update(0.0f)
	{}

	void update(const float dt)
	{
		const float speed = 50.0f;
		last_direction_update += dt;
		position += (dt * speed) * sf::Vector2f(cos(direction), sin(direction));
	}

	void render(sf::RenderTarget& target) const
	{
		const float width = 2.0f;
		const float length = 10.0f;
		sf::RectangleShape body(sf::Vector2f(width, length));
		body.setOrigin(width * 0.5f, length * 0.5f);
		body.setPosition(position);
		body.setRotation(direction * 57.2958f + 90);
		body.setFillColor(sf::Color::Green);

		target.draw(body);
	}

	sf::Vector2f position;
	float direction;
	float last_direction_update;

	const float direction_update_period = 1.0f;
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(1600, 900), "AntSim");
	window.setFramerateLimit(60);

	Ant ant(800, 450, 0.0f);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		ant.update(0.016f);

		window.clear();
		
		ant.render(window);

		window.display();
	}

	return 0;
}