/* ┌────────────────────────────┐
   │ Some simple GPIO managment │
   └────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#pragma once

#include "stm32f3xx_hal.h"
#include <inttypes.h>

#include <bsp/pin.h>


/* ┌────────────────────────────────────────┐
   │ Pin control interface                  │
   └────────────────────────────────────────┘ */

/* Note that the pin struct aren't pointers: placed on stack! */


void    gpio_pin_set   (struct Pin pin, uint8_t value);
uint8_t gpio_pin_get   (struct Pin pin);

void    gpio_led_toggle(void);


/* ┌────────────────────────────────────────┐
   │ Global interface                       │
   └────────────────────────────────────────┘ */

void gpio_init(void);
