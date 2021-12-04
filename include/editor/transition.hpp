#pragma once
#include <chrono>
#include <cmath>

namespace trn
{

template<typename T>
class Transition
{
public:
	using ChronoPoint = std::chrono::steady_clock::time_point;

	Transition()
		: m_start_value()
		, m_current_value()
		, m_target_value()
		, m_delta()
		, m_start_time(std::chrono::steady_clock::now())
		, m_speed(1.0f)
	{
		updateDelta();
	}

	Transition(const T& value, float speed = 1.0f)
		: m_start_value(value)
		, m_current_value(value)
		, m_target_value(value)
		, m_delta()
		, m_last_access(std::chrono::steady_clock::now())
		, m_start_time(std::chrono::steady_clock::now())
		, m_speed(speed)
	{
		updateDelta();
	}

	template<typename... Args>
	explicit Transition(Args&&... args)
		: m_start_value(std::forward<Args>(args)...)
		, m_current_value(m_start_value)
		, m_target_value(m_start_value)
		, m_last_access(std::chrono::steady_clock::now())
		, m_start_time(std::chrono::steady_clock::now())
		, m_speed(1.0f)
		, m_delta()
	{
		updateDelta();
	}

	operator const T&() const
	{
		autoUpdate();
		return m_current_value;
	}

	void operator=(const T& value)
	{
		m_target_value = value;
		restart();
	}

	template<typename U>
	void operator+=(const U& value)
	{
		m_target_value += value;
		restart();
	}

	template<typename U>
	void operator-=(const U& value)
	{
		m_target_value -= value;
		restart();
	}

	template<typename U = T>
	U as() const
	{
        autoUpdate();
		return static_cast<U>(m_current_value);
	}

	void setValueInstant(const T& value)
	{
        m_start_value   = value;
		m_current_value = value;
		m_target_value  = value;
		updateDelta();
	}

	void setSpeed(float s)
	{
		m_speed = s;
	}

	// Instantly moves the current_value to a new one
	void setCurrentValue(const T& new_val)
	{
		m_current_value = new_val;
		updateDelta();
	}

public:
	T m_start_value;
	T m_target_value;
	T m_delta;
	mutable ChronoPoint m_last_access;
	mutable T m_current_value;

	ChronoPoint m_start_time;
	float m_speed;

	void autoUpdate() const
	{
		const ChronoPoint now(std::chrono::steady_clock::now());
		const uint64_t dt(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_last_access).count());

		if (dt > 2)
		{
			m_last_access = now;
			const uint32_t t(static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time).count()));
			if (t > 1)
			{
				m_current_value = m_start_value + m_delta * ratio(t * 0.001f * m_speed);
			}
		}
	}

	static float ratio(float t)
	{
		const float width(5.0f);
		return 1.0f / (1.0f + std::expf(-(width*(2.0f*t - 1.0f))));
	}

	void restart()
	{
		m_start_value = m_current_value;
		m_start_time = std::chrono::steady_clock::now();
        m_last_access = m_start_time;
		updateDelta();
	}

	void updateDelta()
	{
		m_delta = m_target_value - m_start_value;
	}
};



}




