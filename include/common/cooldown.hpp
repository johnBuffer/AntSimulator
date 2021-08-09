#pragma once

struct Cooldown
{
	float target;
	float value;

	Cooldown()
		: target(0.0f)
		, value(0.0f)
	{}

	Cooldown(float t)
		: target(t)
		, value(0.0f)
	{}

	Cooldown(float t, float v)
		: target(t)
		, value(v)
	{}

	void update(float dt)
	{
		value += dt;
	}

	bool updateAutoReset(float dt)
	{
		update(dt);
		bool res = ready();
		if (res) {
			reset();
		}
		return res;
	}

	bool ready() const
	{
		return value >= target;
	}

	bool readyNext(float dt) const
	{
		return value < target && value + dt >= target;
	}
    
    float getRatio() const
    {
        return value / target;
    }

	void reset()
	{
		value = 0.0f;
	}
};
