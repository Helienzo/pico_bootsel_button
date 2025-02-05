/**
 * @file:       pico_bootsel_button.c
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Implementation of bootselect button abstraction
 *
 * @license: MIT License
 *
 * Copyright (c) 2024 Lucas Wennerholm
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Portions of this file (get_bootsel_button function) are:
 * 
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Licensed under the BSD-3-Clause License.
 * See LICENSE.BSD or https://spdx.org/licenses/BSD-3-Clause.html for details.
 *  
*/

#include "pico_bootsel_button.h"
#include "hardware/sync.h"
#include "hardware/structs/ioqspi.h"
#include "hardware/structs/sio.h"
#define BUTTON_DEBOUNCE_COUNT 100

/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
static bool __no_inline_not_in_flash_func(get_bootsel_button)() {
    const uint CS_PIN_INDEX = 1;

    // Must disable interrupts, as interrupt handlers may be in flash, and we
    // are about to temporarily disable flash access!
    uint32_t flags = save_and_disable_interrupts();

    // Set chip select to Hi-Z
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_LOW << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    // Note we can't call into any sleep functions in flash right now
    for (volatile int i = 0; i < 1000; ++i);

    // The HI GPIO registers in SIO can observe and control the 6 QSPI pins.
    // Note the button pulls the pin *low* when pressed.
#if PICO_RP2040
    #define CS_BIT (1u << 1)
#else
    #define CS_BIT SIO_GPIO_HI_IN_QSPI_CSN_BITS
#endif
    bool button_state = !(sio_hw->gpio_hi_in & CS_BIT);

    // Need to restore the state of chip select, else we are going to have a
    // bad time when we return to code in flash!
    hw_write_masked(&ioqspi_hw->io[CS_PIN_INDEX].ctrl,
                    GPIO_OVERRIDE_NORMAL << IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_LSB,
                    IO_QSPI_GPIO_QSPI_SS_CTRL_OEOVER_BITS);

    restore_interrupts(flags);

    return button_state;
}

static inline bool buttonDebounce(picoBootSelButton_t *button, bool pin_state) {
    if (pin_state != button->state) {
        button->counter++;
        if (button->counter > BUTTON_DEBOUNCE_COUNT) {
            button->counter = 0;
            button->state   = pin_state;
            if (pin_state) {
                return true;
            }
        }
    } else if (button->counter > 0){
        button->counter--;
    }

    return false;
}

int32_t picoBootSelButtonInit(picoBootSelButton_t *button, picoBootSelButtonInterface_t *interface) {
    if (button == NULL || interface == NULL) {
        return PICO_BOOTSEL_BTN_NULL_ERROR;
    }

    button->interface = interface;

    return PICO_BOOTSEL_BTN_SUCCESS;
}

int32_t picoBootSelButtonProcess(picoBootSelButton_t *button) {
        // Check the button
        bool press = buttonDebounce(button, get_bootsel_button() ^ PICO_DEFAULT_LED_PIN_INVERTED);
        if (press) {
            if (button->interface != NULL && button->interface->event_cb != NULL) {
                // Call callback
                button->interface->event_cb(button->interface, PICO_BOOTSEL_BTN_SHORT_PRESS);
            }
        }

    return PICO_BOOTSEL_BTN_SUCCESS;
}


