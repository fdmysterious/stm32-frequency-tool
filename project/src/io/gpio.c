/* ┌────────────────────────────┐
   │ Some simple GPIO managment │
   └────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/


#include "gpio.h"
#include "stm32f3xx_hal.h"

/* ┌────────────────────────────────────────┐
   │ GPIO defines                           │
   └────────────────────────────────────────┘ */

struct GPIO_Pin PIN_LD2 = { .port = GPIOB, .pin = GPIO_PIN_13 };


/* ┌────────────────────────────────────────┐
   │ Pin control interface                  │
   └────────────────────────────────────────┘ */

void gpio_pin_set(struct GPIO_Pin pin, uint8_t value)
{
	HAL_GPIO_WritePin(pin.port, pin.pin, value);
}

uint8_t gpio_pin_get(struct GPIO_Pin pin)
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
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/* Init LD2 */
	init.Pin   = PIN_LD2.pin;
	init.Mode  = GPIO_MODE_OUTPUT_PP;
	init.Pull  = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(PIN_LD2.port, &init);
}
