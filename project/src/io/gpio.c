/* ┌────────────────────────────┐
   │ Some simple GPIO managment │
   └────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/


#include "gpio.h"
#include "stm32f3xx_hal.h"

#include <bsp/pin.h>


/* ┌────────────────────────────────────────┐
   │ Pin control interface                  │
   └────────────────────────────────────────┘ */

void gpio_pin_set(struct Pin pin, uint8_t value)
{
	HAL_GPIO_WritePin(pin.port, pin.pin, value);
}

uint8_t gpio_pin_get(struct Pin pin)
{
	return HAL_GPIO_ReadPin(pin.port, pin.pin);
}


/* ┌────────────────────────────────────────┐
   │ GPIOs init                             │
   └────────────────────────────────────────┘ */

void gpio_init(void)
{
	GPIO_InitTypeDef init;

	/* GPIO enable port clocks */

	/* Init LD2 */
	init.Pin   = PIN_LD2.pin;
	init.Mode  = GPIO_MODE_OUTPUT_PP;
	init.Pull  = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PIN_LD2.port, &init);
}

void gpio_led_toggle(void)
{
	static uint8_t ld_state = 0;
	ld_state = 1 - ld_state;
	gpio_pin_set(PIN_LD2, ld_state);
}
