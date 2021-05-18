#include <vector>


template<typename T>
struct RAccBase
{
    uint32_t max_values_count;
    std::vector<T> values;
    uint32_t current_index;
    T pop_value;

    RAccBase(uint32_t max_size=8)
        : max_values_count(max_size)
        , values(max_size)
        , current_index(0)
        , pop_value(0.0f)
    {
    }

    bool addValueBase(T val)
    {
        const bool pop = current_index >= max_values_count;
        const uint32_t i = current_index % max_values_count;
        pop_value = values[i];
        values[i] = val;
        ++current_index;
        return pop;
    }

    uint32_t getCount() const
    {
        return std::min(current_index, max_values_count);
    }

    virtual T get() const = 0;

    operator T() const
    {
        return get();
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

