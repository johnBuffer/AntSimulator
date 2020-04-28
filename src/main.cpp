#include <SFML/Graphics.hpp>
#include <vector>


constexpr float PI = 3.14159265f;


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
		position += (dt * speed) * sf::Vector2f(cos(direction), sin(direction));

		last_direction_update += dt;
		if (last_direction_update >= direction_update_period) {
			findNewDirection();
		}
	}

	void findNewDirection()
	{
		const float range = PI * 0.5f;
		direction += getRandRange(range);
		last_direction_update = 0.0f;
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


struct Colony
{
	Colony(float x, float y, uint32_t n)
	{
		for (uint32_t i(n); i--;) {
			ants.emplace_back(x, y, getRandRange(2.0f * PI));
		}
	}

	void update(const float dt)
	{
		for (Ant& a : ants) {
			a.update(dt);
		}
	}

	void render(sf::RenderTarget& target) const
	{
		for (const Ant& a : ants) {
			a.render(target);
		}
	}

	std::vector<Ant> ants;
};


int main()
{
	sf::RenderWindow window(sf::VideoMode(1600, 900), "AntSim");
	window.setFramerateLimit(60);

	Colony colony(800, 450, 100);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		colony.update(0.016f);

		window.clear();
		
		colony.render(window);

		window.display();
	}

	return 0;
}