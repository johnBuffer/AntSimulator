#pragma once
#include <random>


class NumberGenerator
{
protected:
	std::random_device rd;
	std::mt19937 gen;

	NumberGenerator()
		: gen(0)
	{}
};


template<typename T>
class RealNumberGenerator : public NumberGenerator
{
private:
	std::uniform_real_distribution<T> dis;

public:
	RealNumberGenerator()
		: NumberGenerator()
		, dis(0.0f, 1.0f)		
	{}
	
	// random_device is not copyable
	RealNumberGenerator(const RealNumberGenerator<T>& right)
		: NumberGenerator()
		, dis(right.dis)
	{}

	float get()
	{
		return dis(gen);
	}
    
    bool proba(float p = 0.5f)
    {
        return get() < p;
    }

	float getUnder(T max)
	{
		return get() * max;
	}

	float getRange(T min, T max)
	{
		return min + get() * (max - min);
	}

	float getRange(T width)
	{
		return getRange(-width * 0.5f, width * 0.5f);
	}
    
    float getFullRange(T width)
    {
        return getRange(2.0f * width);
    }
};


using RandFloatGenerator = RealNumberGenerator<float>;
