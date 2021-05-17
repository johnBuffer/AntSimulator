#include <vector>


template<typename T>
struct RAccBase
{
    uint32_t max_values_count;
    std::vector<T> values;
    uint32_t current_index;
    T pop_value;

    RAccBase(uint32_t max_size=8)
        : values(max_size)
        , current_index(0)
    {
    }

    bool addValueBase(T val)
    {
        const uint32_t i = current_index % max_values_count;
        pop_value = values[i];
        values[i] = val;
        ++current_index;
    }

    virtual bool addValue() = 0;
    virtual T get() const = 0;
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


};

