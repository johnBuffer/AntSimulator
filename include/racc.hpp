#pragma once
#include <vector>
#include "utils.hpp"


template<typename T>
struct RAccBase
{
    uint32_t max_values_count;
    std::vector<T> values;
    uint32_t current_index;
    T pop_value;

    RAccBase(uint32_t max_size=8)
        : max_values_count(max_size)
        , values(max_size, to<T>(0.0f))
        , current_index(0)
        , pop_value(to<T>(0.0f))
    {
    }

    bool addValueBase(T val)
    {
        const bool pop = current_index >= max_values_count;
        const uint32_t i = getIndex();
        pop_value = values[i];
        values[i] = val;
        ++current_index;
        return pop;
    }

    uint32_t getCount() const
    {
        return std::min(current_index + 1, max_values_count);
    }

    virtual T get() const = 0;

    operator T() const
    {
        return get();
    }

protected:
    uint32_t getIndex(int32_t offset = 0) const
    {
        return (current_index + offset) % max_values_count;
    }
};


template<typename T>
struct RMean : public RAccBase<T>
{
    T sum;

    RMean(uint32_t max_size=8)
        : RAccBase<T>(max_size)
        , sum(0.0f)
    {
    }

    void addValue(T v)
    {
        sum += v - float(RAccBase<T>::addValueBase(v)) * RAccBase<T>::pop_value;
    }

    T get() const override
    {
        return sum / float(RAccBase<T>::getCount());
    }
};


template<typename T>
struct RDiff : public RAccBase<T>
{
    RDiff(uint32_t max_size = 8)
        : RAccBase<T>(max_size)
    {
    }

    void addValue(T v)
    {
        RAccBase<T>::addValueBase(v);
    }

    T get() const override
    {
        return RAccBase<T>::values[RAccBase<T>::getIndex(-1)] - RAccBase<T>::values[RAccBase<T>::getIndex()];
    }
};
