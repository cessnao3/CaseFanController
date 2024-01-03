#pragma once

class FanController
{
    const int pin_a;
    const int pin_b;
    const int pin_pwm;

    int current_speed = 0;
    bool reversed = false;

    static const int MAX_SPEED = 255;

public:
    FanController(const int pin_a, const int pin_b, const int pin_pwm);

    void set_speed(int new_speed);

    int get_speed() const;

    void set_reversed(const bool rev);

    bool get_reversed() const;
};
