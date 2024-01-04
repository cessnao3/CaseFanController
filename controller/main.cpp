#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include <atomic>

#include "lookup.hpp"
#include "controller.hpp"

static const int PIN_LED = 25;
static const int PIN_MOTOR_STANDBY = 2;
static const int PIN_MOTOR_1_A = 0;
static const int PIN_MOTOR_1_B = 1;
static const int PIN_MOTOR_1_PWM = 12;
static const int PIN_MOTOR_2_A = 10;
static const int PIN_MOTOR_2_B = 11;
static const int PIN_MOTOR_2_PWM = 13;

static const int PIN_MB_PWM_IN = 19;

static uint64_t pwm_last_clock = 0;
static uint64_t pwm_pulse_width = 0;

void pulse_width_callback(uint gpio, uint32_t event_mask)
{
    if (gpio == PIN_MB_PWM_IN)
    {
        if ((event_mask & GPIO_IRQ_EDGE_RISE) != 0)
        {
            pwm_last_clock = time_us_64();
        }

        if ((event_mask & GPIO_IRQ_EDGE_FALL) != 0)
        {
            pwm_pulse_width = time_us_64() - pwm_last_clock;
        }
    }
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
    gpio_set_dir(PIN_MB_PWM_IN, GPIO_IN);

    gpio_set_irq_enabled_with_callback(PIN_MB_PWM_IN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, pulse_width_callback);

    // Create the motor controller
    MotorController controller_a(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_MOTOR_1_PWM);
    MotorController controller_b(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_MOTOR_2_PWM, &controller_a);

    // Main Loop
    while (1)
    {
        gpio_put(PIN_LED, 1);
        sleep_ms(500);

        gpio_put(PIN_LED, 0);
        sleep_ms(500);

        controller_a.set_speed(100);
        controller_b.set_speed(200);

        printf("Pulse Width: %d\n", pwm_pulse_width);
    }

    // Return result
    return 0;
}
