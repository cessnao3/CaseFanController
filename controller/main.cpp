#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

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

    // Create the motor controller
    FanController controller_a(PIN_MOTOR_1_A, PIN_MOTOR_1_B, PIN_MOTOR_1_PWM);
    FanController controller_b(PIN_MOTOR_2_A, PIN_MOTOR_2_B, PIN_MOTOR_2_PWM);

    // Main Loop
    while (1)
    {
        gpio_put(PIN_LED, 1);
        printf("LED ON!\n");
        sleep_ms(500);

        gpio_put(PIN_LED, 0);
        printf("LED OFF!\n");
        sleep_ms(500);

        controller_a.set_speed(100);
        controller_b.set_speed(200);
    }

    // Return result
    return 0;
}
