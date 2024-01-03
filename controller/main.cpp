#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include "lookup.hpp"
#include "controller.hpp"

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
