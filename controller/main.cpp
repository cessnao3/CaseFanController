#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include "controller.hpp"

#include "hardware/pwm.h"

static const int PIN_LED = 25;
static const int PIN_MOTOR_STANDBY = 2;
static const int PIN_MOTOR_1_A = 0;
static const int PIN_MOTOR_1_B = 1;
static const int PIN_MOTOR_1_PWM = 12;
static const int PIN_MOTOR_2_A = 10;
static const int PIN_MOTOR_2_B = 11;
static const int PIN_MOTOR_2_PWM = 13;

static const int PIN_MB_PWM_IN = 19;

float measure_duty_cycle(uint gpio)
{
    // Only the PWM B pins can be used as inputs.
    if (pwm_gpio_to_channel(gpio) != PWM_CHAN_B)
    {
        return -1.0f;
    }

    uint slice_num = pwm_gpio_to_slice_num(gpio);

    // Count once for every 100 cycles the PWM B input is high
    pwm_config cfg = pwm_get_default_config();
    pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_HIGH);
    pwm_config_set_clkdiv(&cfg, 100);
    pwm_init(slice_num, &cfg, false);
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    pwm_set_enabled(slice_num, true);
    sleep_ms(10);
    pwm_set_enabled(slice_num, false);
    float counting_rate = clock_get_hz(clk_sys) / 100;
    float max_possible_count = counting_rate * 0.01;
    return pwm_get_counter(slice_num) / max_possible_count;
}

uint64_t read_pulse_width(const uint gpio)
{
    while (!gpio_get(gpio))
    {
    }
    uint64_t t_init = time_us_64();
    sleep_us(2);
    while (gpio_get(gpio))
    {
    }
    return time_us_64() - t_init;
}

template <typename T>
T ctl_clamp(const T val, const T lower_bound, const T upper_bound)
{
    if (val > upper_bound)
    {
        return upper_bound;
    }
    else if (val < lower_bound)
    {
        return lower_bound;
    }
    else
    {
        return val;
    }
}

int main()
{
    // Init the pico
    stdio_init_all();

    // initialize GPIO (Green LED connected to pin 25)
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    // Set motor enable
    gpio_init(PIN_MOTOR_STANDBY);
    gpio_set_dir(PIN_MOTOR_STANDBY, GPIO_OUT);
    gpio_put(PIN_MOTOR_STANDBY, 1);

    // Set PWM input
    gpio_init(PIN_MB_PWM_IN);
    gpio_set_dir(PIN_MB_PWM_IN, GPIO_FUNC_PWM);

    // Create the motor controller
    MotorController controller_a(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_MOTOR_1_PWM);
    MotorController controller_b(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_MOTOR_2_PWM, &controller_a);

    // Main Loop
    bool led_setting = true;
    int print_interval = 0;

    int speed_command = 0;
    int rate_limited_command = 0;

    const int SPEED_LIMIT = 50;
    const int SLEEP_MS = 50;
    const int RATE_HZ = 1000 / SLEEP_MS;

    while (1)
    {
        gpio_put(PIN_LED, led_setting);
        sleep_ms(SLEEP_MS);
        led_setting = !led_setting;

        const float duty_cycle = measure_duty_cycle(PIN_MB_PWM_IN);
        speed_command = ctl_clamp(
            static_cast<int>(MotorController::MAX_SPEED * duty_cycle),
            0,
            MotorController::MAX_SPEED);

        const int speed_delta = ctl_clamp(speed_command - rate_limited_command, -SPEED_LIMIT, SPEED_LIMIT);
        rate_limited_command += speed_delta;

        int used_speed = rate_limited_command;

        if (used_speed < 0.1f * MotorController::MAX_SPEED)
        {
            used_speed = 0;
        }

        controller_a.set_speed(used_speed);
        controller_b.set_speed(used_speed);

        if ((++print_interval) % RATE_HZ == 0)
        {
            printf("DC: %.2f - %d (CMD %d, Rate Limited %d)\n", duty_cycle, used_speed, speed_command, rate_limited_command);
        }
    }

    // Return result
    return 0;
}
