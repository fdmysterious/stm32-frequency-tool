/* ┌─────────────────┐
   │ Error managment │
   └─────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "error.h"

#include <bsp/pin.h>
#include <io/gpio.h>

void error_handler(void)
{
	while(1) {
		gpio_pin_set(PIN_LD2, 1);
		HAL_Delay(100);
		gpio_pin_set(PIN_LD2, 0);
		HAL_Delay(100);
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
}
