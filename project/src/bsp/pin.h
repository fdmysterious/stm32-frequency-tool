/* ┌─────────────────┐
   │ Pins definition │
   └─────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#pragma once

#include "stm32f3xx_hal.h"
#include "inttypes.h"


/* ┌────────────────────────────────────────┐
   │ Structs for pins                       │
   └────────────────────────────────────────┘ */

struct Pin {
	GPIO_TypeDef *port;
	uint16_t       pin;
};


/* ┌────────────────────────────────────────┐
   │ External pins definition               │
   └────────────────────────────────────────┘ */

extern const struct Pin PIN_LD2;

extern const struct Pin PIN_USART_TX;
extern const struct Pin PIN_USART_RX;

extern const struct Pin PIN_PWM1_OUT;
extern const struct Pin PIN_PWM2_OUT;
extern const struct Pin PIN_PWM3_OUT;
extern const struct Pin PIN_PWM4_OUT;

extern const struct Pin PIN_FMETER1_IN;
extern const struct Pin PIN_FMETER2_IN;
extern const struct Pin PIN_FMETER3_IN;

extern const struct Pin PIN_ACOMP_IN;
