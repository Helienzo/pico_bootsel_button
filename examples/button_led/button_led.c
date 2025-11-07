#include <stdio.h>
#include "pico/stdlib.h"
#include "pico_bootsel_button.h"

// Create a LOG function that can be used to print over UART
#ifndef LOG
#define LOG(f_, ...) printf((f_), ##__VA_ARGS__)
#endif

// Use a custom GPIO as LED
#define CUSTOM_LED_GPIO 13

static void device_error();

// Declare the myInstance type
typedef struct {
   // Button management
    picoBootSelButton_t          boot_button;
    picoBootSelButtonInterface_t btn_interface;
    // LED management
    bool test_led_state;
} myInstance_t;

// Create one instance of myInstance and initialize it to zeros
static myInstance_t my_instance = {0};

// Turn the led on or off
void pico_set_led(bool led_on) {
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    gpio_put(CUSTOM_LED_GPIO, led_on);
}

// Perform initialisation
int pico_led_init(void) {
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(CUSTOM_LED_GPIO);
    gpio_set_dir(CUSTOM_LED_GPIO, GPIO_OUT);
    return PICO_OK;
}

void buttonEventCb(picoBootSelButtonInterface_t *interface, picoBootSelButtonEvent_t event) {
    // Get the instance pointer from the interface
    myInstance_t * inst = CONTAINER_OF(interface, myInstance_t, btn_interface);

    inst->test_led_state = !inst->test_led_state;
    pico_set_led(inst->test_led_state);
    LOG("BUTTON PRESSED!\n");
}

int main()
{
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);

    // Prepare bootsel button
    my_instance.btn_interface.event_cb = buttonEventCb;
    int32_t res = picoBootSelButtonInit(&my_instance.boot_button, &my_instance.btn_interface);
    if (res != PICO_BOOTSEL_BTN_SUCCESS) {
        LOG("BUTTON INIT FAILED!\n");
        device_error();
    }

    while (true) {
        // Process the button
        res = picoBootSelButtonProcess(&my_instance.boot_button);
        if (res != PICO_BOOTSEL_BTN_SUCCESS) {
            LOG("BUTTON PROCESS FAILED!\n");
            device_error();
        }
    }
}

static void device_error() {
    // Forever blink fast
    while (true) {
        pico_set_led(true);
        sleep_ms(100);
        pico_set_led(false);
        sleep_ms(100);
    }
}