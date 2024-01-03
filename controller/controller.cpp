#include "controller.hpp"

FanController::FanController(const int pin_a, const int pin_b, const int pin_pwm) : pin_a{pin_a}, pin_b{pin_b}, pin_pwm{pin_pwm}
{
    // Initialize
}

void FanController::set_speed(int new_speed)
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
    {
        // TODO - Somewhere, force direction setting to initiailize pins at the beginning
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

int FanController::get_speed() const
{
    return current_speed;
}

void FanController::set_reversed(const bool rev)
{
    reversed = rev;
    set_speed(current_speed);
}

bool FanController::get_reversed() const
{
    return reversed;
}
