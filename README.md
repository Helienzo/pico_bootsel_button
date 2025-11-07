# Pico BOOTSEL Button Library

Use the Raspberry Pi Pico's BOOTSEL button as a regular input button during program execution.

## Features

- Read BOOTSEL button during normal execution
- Event-driven callback system
- Built-in debouncing
- Compatible with RP2040 and RP2350
- Implements CONTAINER_OF idiom

## Usage

```c
#include "pico_bootsel_button.h"

// Define your application structure
typedef struct {
    picoBootSelButton_t          bootButton;   // Button state
    picoBootSelButtonInterface_t btnInterface; // Button interface with callback
    int myData;                                 // Your application data
} myApp_t;

static myApp_t app = {0};

// Button press callback function
void buttonCallback(picoBootSelButtonInterface_t *interface,
                    picoBootSelButtonEvent_t event) {
    // Use CONTAINER_OF to retrieve your application structure from the interface
    myApp_t *app = CONTAINER_OF(interface, myApp_t, btnInterface);
    app->myData++;  // Access and modify your application data
}

int main() {
    // Register the callback function
    app.btnInterface.event_cb = buttonCallback;

    // Initialize the button with the interface
    picoBootSelButtonInit(&app.bootButton, &app.btnInterface);

    while (true) {
        // Poll the button state - calls callback on button press
        picoBootSelButtonProcess(&app.bootButton);
    }
}
```

See `examples/button_led/` for a complete example.
