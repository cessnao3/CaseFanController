#include "controller.hpp"

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pwm.h"

#include <cmath>

MotorController::MotorController(const int pin_a, const int pin_b, const int pin_pwm, MotorController* linked_controller) : pin_a{pin_a}, pin_b{pin_b}, pin_pwm{pin_pwm}
{
    gpio_init(pin_a);
    gpio_set_dir(pin_a, GPIO_OUT);

    gpio_init(pin_b);
    gpio_set_dir(pin_b, GPIO_OUT);

    gpio_init(pin_pwm);
    gpio_set_function(pin_pwm, GPIO_FUNC_PWM);

    // Setup the PWM output, and only init the slice if necessary based on the linked controller
    const auto slice = pwm_gpio_to_slice_num(pin_pwm);
    if (linked_controller != nullptr || pwm_gpio_to_slice_num(linked_controller->pin_pwm) != slice)
    {
        pwm_set_wrap(slice, PWM_WRAP);
        pwm_set_enabled(slice, true);

        // TargetFreq = SystemClock / ClockDivide / PwmRange
        // ClockDivide = SystemClock / TargetFreq / PwmRange
        const float clock_divide = (float)clock_get_hz(clk_sys) / (500.0f) / (float)PWM_WRAP;
        pwm_set_clkdiv(slice, clock_divide);
    }

    // Set the GPIO level
    pwm_set_gpio_level(pin_pwm, 0);
}

void MotorController::set_speed(int new_speed, const bool force)
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

    if (new_speed != current_speed || force)
    {
        // Update direction pins
        if (sign(new_speed) != sign(current_speed) || force)
        {
            if (new_speed > 0)
            {
                gpio_put(pin_a, 1);
                gpio_put(pin_b, 0);
            }
            else if (new_speed < 0)
            {
                gpio_put(pin_a, 0);
                gpio_put(pin_b, 1);
            }
            else if (new_speed == 0)
            {
                gpio_put(pin_a, 0);
                gpio_put(pin_b, 0);
            }
        }

        // Set the GPIO level
        pwm_set_gpio_level(pin_pwm, std::abs(new_speed));

        // Update the final speed
        current_speed = new_speed;
    }
}

int MotorController::get_speed() const
{
    return current_speed;
}

void MotorController::set_reversed(const bool rev)
{
    reversed = rev;
    set_speed(current_speed);
}

bool MotorController::get_reversed() const
{
    return reversed;
}

int MotorController::sign(int val)
{
    if (val < 0)
    {
        return -1;
    }
    else if (val > 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
