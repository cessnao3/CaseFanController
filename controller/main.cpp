#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include <atomic>

#include "lookup.hpp"
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

static const int PIN_MB_PWM_IN = 18;
static const int PIN_MB_TACH_OUT = 19;

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

int main()
{
    // Init the pico
    stdio_init_all();

    // initialise GPIO (Green LED connected to pin 25)
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    // Set motor enable
    gpio_init(PIN_MOTOR_STANDBY);
    gpio_set_dir(PIN_MOTOR_STANDBY, GPIO_OUT);
    gpio_put(PIN_MOTOR_STANDBY, 1);

    // Set PWM input
    gpio_init(PIN_MB_PWM_IN);
    gpio_set_dir(PIN_MB_PWM_IN, GPIO_FUNC_PWM);

    gpio_init(PIN_MB_TACH_OUT);
    gpio_set_dir(PIN_MB_TACH_OUT, GPIO_FUNC_PWM);

    // Create the motor controller
    MotorController controller_a(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_MOTOR_1_PWM);
    MotorController controller_b(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_MOTOR_2_PWM, &controller_a);

    // gpio_set_irq_enabled_with_callback(PIN_MB_PWM_IN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, pulse_width_callback);

    const LookupTable lookup({
        {0.00f, static_cast<int>(MotorController::MAX_SPEED * 0.3f)},
        {0.30f, static_cast<int>(MotorController::MAX_SPEED * 0.3f)},
        {0.40f, static_cast<int>(MotorController::MAX_SPEED * 0.4f)},
        {0.50f, static_cast<int>(MotorController::MAX_SPEED * 0.6f)},
        {0.60f, static_cast<int>(MotorController::MAX_SPEED * 0.8f)},
        {0.70f, static_cast<int>(MotorController::MAX_SPEED * 1.0f)},
    });

    bool led_setting = true;

    // Main Loop
    while (1)
    {
        gpio_put(PIN_LED, led_setting);
        sleep_ms(500);
        led_setting = !led_setting;

        const auto dc = measure_duty_cycle(PIN_MB_TACH_OUT);
        const auto new_speed = lookup.get_speed(dc);

        controller_a.set_speed(new_speed);
        controller_b.set_speed(new_speed);

        printf("Duty-Cycle: %.2f - %d\n", dc, new_speed);
    }

    // Return result
    return 0;
}
