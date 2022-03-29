/* ┌───────────────────────┐
   │ Simple UART managment │
   └───────────────────────┘
   
    Florian Dupeyron
    March 2022

	→ For the UART module, the LL driver is used instead of HAL
*/

#pragma once

#include "stm32f3xx_hal.h"

void uart_init(void);
void uart_hello(void);
