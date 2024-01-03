#pragma once

class FanController
{
    const int pin_a;
    const int pin_b;
    const int pin_pwm;

    int current_speed = 0;
    bool reversed = false;

    static const int PWM_WRAP = 1024;
    static const int MAX_SPEED = PWM_WRAP - 1;

public:
    FanController(const int pin_a, const int pin_b, const int pin_pwm);

    void set_speed(int new_speed, bool force = false);

    int get_speed() const;

    void set_reversed(const bool rev);

    bool get_reversed() const;

private:
    static int sign(int val);
};
