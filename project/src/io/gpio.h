/* ┌────────────────────────────┐
   │ Some simple GPIO managment │
   └────────────────────────────┘
   
    Florian Dupeyron
    March 2022

	TODO:
	- Missing support for Alternate functions
*/

#pragma once

#include "stm32f3xx_hal.h"
#include <inttypes.h>


/* ┌────────────────────────────────────────┐
   │ Definitions for GPIO pins              │
   └────────────────────────────────────────┘ */

struct GPIO_Pin {
	GPIO_TypeDef *port;
	uint16_t       pin;
};

/* ┌────────────────────────────────────────┐
   │ External pins definition               │
   └────────────────────────────────────────┘ */

extern const struct GPIO_Pin PIN_LD2;

extern const struct GPIO_Pin PIN_USART_TX;
extern const struct GPIO_Pin PIN_USART_RX;

extern const struct GPIO_Pin PIN_PWM1_OUT;
extern const struct GPIO_Pin PIN_PWM2_OUT;
extern const struct GPIO_Pin PIN_PWM3_OUT;

extern const struct GPIO_Pin PIN_FMETER1_IN;
extern const struct GPIO_Pin PIN_FMETER2_IN;
extern const struct GPIO_Pin PIN_FMETER3_IN;


/* ┌────────────────────────────────────────┐
   │ Pin control interface                  │
   └────────────────────────────────────────┘ */

/* Note that the pin struct aren't pointers: placed on stack! */


void    gpio_pin_set(struct GPIO_Pin pin, uint8_t value);
uint8_t gpio_pin_get(struct GPIO_Pin pin);

void gpio_led_toggle(void);


/* ┌────────────────────────────────────────┐
   │ Global interface                       │
   └────────────────────────────────────────┘ */

void gpio_init(void);
