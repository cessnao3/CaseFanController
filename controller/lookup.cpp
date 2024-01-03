#include "lookup.hpp"

LookupTable::LookupTable(std::initializer_list<std::pair<int, int>> init)
{
    for (const auto &[x, y] : init)
    {
        vals.push_back({x, y});
    }
}

void LookupTable::add_speed(const int pwm, const int speed)
{
    size_t add_index = 0;

    for (size_t i = 0; i < vals.size(); ++i)
    {
        const auto current = vals[i].first;

        if (pwm == current)
        {
            vals[i].second = speed;
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
