#pragma once
#include <random>


template<typename T>
class NumberGenerator
{
private:
	std::random_device rd;
	std::uniform_real_distribution<T> dis;
	std::mt19937 gen;

public:
	NumberGenerator()
		: rd()
		, dis(0.0f, 1.0f)
		, gen(rd())
		
	{}
	
	// random_device is not copyable
	NumberGenerator(const NumberGenerator<T>& right)
		: rd()
		, dis(right.dis)
		, gen(rd())

	{}

	float get()
	{
		return dis(gen);
	}

	float getUnder(float max)
	{
		return get() * max;
	}

	float getRange(float min, float max)
	{
		return min + get() * (max - min);
	}

	float getRange(float width)
	{
		return getRange(-width * 0.5f, width * 0.5f);
	}
};


template<typename T>
class RNG
{
private:
	static NumberGenerator<T> gen;

public:
	static T get()
	{
		return gen.get();
	}

	static float getUnder(float max)
	{
		return gen.getUnder(max);
	}

	static float getRange(float min, float max)
	{
		return gen.getRange(min, max);
	}

	static float getRange(float width)
	{
		return gen.getRange(width);
	}

	static float getFullRange(float width)
	{
		return gen.getRange(2.0f * width);
	}

	static bool proba(float threshold)
	{
		return get() < threshold;
	}
};

using RNGf = RNG<float>;

template<typename T>
NumberGenerator<T> RNG<T>::gen = NumberGenerator<T>();
