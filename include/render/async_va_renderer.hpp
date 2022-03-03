#pragma once
#include <thread>
#include <mutex>
#include <SFML/Graphics.hpp>
#include "common/double_buffer.hpp"


struct AsyncRenderer
{
	DoubleObject<sf::VertexArray>& vertex_array;
	std::thread thread;
	std::mutex mutex;
	bool run;
	bool swap_ok;

    explicit
	AsyncRenderer(DoubleObject<sf::VertexArray>& target)
		: vertex_array(target)
		, run(true)
		, swap_ok(true)
	{}

	// To be overloaded
	virtual void initializeVertexArray(sf::VertexArray& va) = 0;
	virtual void updateVertexArray() = 0;

	// Start the rendering in the background
	void start()
	{
		initializeVertexArray(vertex_array.getCurrent());
		initializeVertexArray(vertex_array.getLast());
		thread = std::thread([this]() {update(); });
	}

	virtual ~AsyncRenderer()
	{
		run = false;
		thread.join();
	}

private:
	void update()
	{
		while (run) {
			if (swap_ok) {
				updateVertexArray();
			}
			trySwap();
		}
	}

	void trySwap()
	{
		if (mutex.try_lock()) {
			swap_ok = true;
			vertex_array.swap();
			mutex.unlock();
		}
		else {
			swap_ok = false;
		}
	}
};
