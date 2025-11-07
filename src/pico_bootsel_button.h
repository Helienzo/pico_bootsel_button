/**
 * @file:       pico_bootsel_button.h
 * @author:     Lucas Wennerholm <lucas.wennerholm@gmail.com>
 * @brief:      Header file for bootselect button abstraction
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
*/

#ifndef PICO_BOOTSEL_BUTTON_H
#define PICO_BOOTSEL_BUTTON_H
#ifdef __cplusplus
extern "C" {
#endif
#include "pico/stdlib.h"

#ifndef CONTAINER_OF
#define CONTAINER_OF(ptr, type, member)	(type *)((char *)(ptr) - offsetof(type,member))
#endif

typedef enum {
    PICO_BOOTSEL_BTN_SUCCESS,
    PICO_BOOTSEL_BTN_NULL_ERROR    = -1,
    PICO_BOOTSEL_BTN_GEN_ERROR     = -2,
} picoBootSelButtonErr_t;

typedef enum {
    PICO_BOOTSEL_BTN_SHORT_PRESS,
} picoBootSelButtonEvent_t;

typedef struct picoBootSelButtonInterface picoBootSelButtonInterface_t;
typedef void (*picoBootSelButtonEventCB_t)(picoBootSelButtonInterface_t *interface, picoBootSelButtonEvent_t event);

struct picoBootSelButtonInterface {
    picoBootSelButtonEventCB_t event_cb;
};

typedef struct {
    uint32_t counter;
    bool     state;

    picoBootSelButtonInterface_t *interface;
} picoBootSelButton_t;

/**
 * Init the pico bootsel button
 * Input: Pointer to button instance
 * Input: Pointer to button interface
 * Returns: picoBootSelButtonErr_t
 */
int32_t picoBootSelButtonInit(picoBootSelButton_t *button, picoBootSelButtonInterface_t *interface);

/**
 * Process the pico bootsel button
 * Input: Pointer to button instance
 * Returns: picoBootSelButtonErr_t
 */
int32_t picoBootSelButtonProcess(picoBootSelButton_t *button);

#ifdef __cplusplus
}
#endif
#endif /* PICO_BOOTSEL_BUTTON_H */