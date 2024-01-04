#include "lookup.hpp"

LookupTable::LookupTable(std::initializer_list<std::pair<float, int>> init)
{
    for (const auto &[x, y] : init)
    {
        add_speed(x, y);
    }
}

void LookupTable::add_speed(const float pwm, const int speed)
{
    size_t add_index;
    for (add_index = 0; add_index < vals.size(); ++add_index)
    {
        const auto current = vals[add_index].first;

        if (pwm == current)
        {
            vals[add_index].second = speed;
            return;
        }
        else if (pwm < current)
        {
            break;
        }
    }

    vals.insert(vals.begin() + add_index, {pwm, speed});
}

int LookupTable::get_speed(const int x) const
{
    // Return 0 by default if no values provided
    if (vals.size() == 0)
    {
        return 0;
    }

    // Return the first value for sub-parameter items
    if (x < vals.front().first)
    {
        return vals.front().second;
    }

    // Search for a value within the liast
    for (size_t i = 1; i < vals.size(); ++i)
    {
        const auto &n0 = vals[i - 1];
        const auto &n1 = vals[i];

        if (x >= n0.first && x < n1.first)
        {
            return n0.second;
        }
    }

    // No value provided, we must be beyond the end of the list now
    return vals.back().second;
}
