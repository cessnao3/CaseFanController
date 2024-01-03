#pragma once

#include <vector>
#include <utility>

class LookupTable
{
    std::vector<std::pair<int, int>> vals;

public:
    LookupTable(std::initializer_list<std::pair<int, int>> init);

    void add_speed(const int pwm, const int speed);

    int get_speed(const int x) const;
};
