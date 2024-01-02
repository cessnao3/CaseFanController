#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include <array>
#include <cstdint>
#include <optional>
#include <vector>

class FanController
{
    const int pin_a;
    const int pin_b;
    const int pin_pwm;

    int current_speed = 0;
    bool reversed = false;

    static const int MAX_SPEED = 255;

public:
    FanController(const int pin_a, const int pin_b, const int pin_pwm) : pin_a{pin_a}, pin_b{pin_b}, pin_pwm{pin_pwm}
    {
        // Initialize
    }

    void set_speed(int new_speed)
    {
        if (new_speed == current_speed)
        {
            return;
        }

        if (reversed)
        {
            new_speed *= -1;
        }

        if (new_speed > MAX_SPEED)
        {
            new_speed = MAX_SPEED;
        }
        else if (new_speed < -MAX_SPEED)
        {
            new_speed = -MAX_SPEED;
        }

        if (new_speed >= 0 && current_speed < 0)
        { // TODO - Somewhere, force direction setting to initiailize pins at the beginning
          // TODO Switch to Forward
        }
        else if (new_speed < 0 && current_speed > 0)
        {
            // TODO Switch to Reverse
        }

        // TODO - Set PWM parameter here

        // Update the final speed
        current_speed = new_speed;
    }

    int get_speed() const
    {
        return current_speed;
    }

    void set_reversed(const bool rev)
    {
        reversed = rev;
        set_speed(current_speed);
    }

    bool get_reversed() const
    {
        return reversed;
    }
};

class LookupTable
{
    std::vector<std::pair<int, int>> vals;

public:
    LookupTable(std::initializer_list<std::pair<int, int>> init)
    {
        for (const auto &[x, y] : init)
        {
            vals.push_back({x, y});
        }
    }

    void add_speed(const int pwm, const int speed)
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

    int get_speed(const int x) const
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
};

int main()
{
    // Init the pico
    stdio_init_all();

    // initialise GPIO (Green LED connected to pin 25)
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    // Main Loop
    while (1)
    {
        gpio_put(25, 1); // Set pin 25 to high
        printf("LED ON!\n");
        sleep_ms(1000); // 0.5s delay

        gpio_put(25, 0); // Set pin 25 to low
        printf("LED OFF!\n");
        sleep_ms(1000); // 0.5s delay
    }

    // REturn result
    return 0;
}
