#pragma once

#include <vector>
#include <utility>

class LookupTable
{
    std::vector<std::pair<float, int>> vals;

public:
    LookupTable(std::initializer_list<std::pair<float, int>> init);

    void add_speed(const float, const int speed);

    int get_speed(const int x) const;
};
