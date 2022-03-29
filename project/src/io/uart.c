/* ┌───────────────────────┐
   │ Simple UART managment │
   └───────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "uart.h"
#include "stm32f3xx_hal.h"
#include "gpio.h"

#include <func/error.h>

/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

static UART_HandleTypeDef huart2;

/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void uart_init(void)
{
	RCC_PeriphCLKInitTypeDef clk_conf = {0};
	GPIO_InitTypeDef         gpio_conf;

	/* Clock configure */
	clk_conf.PeriphClockSelection = RCC_PERIPHCLK_USART2;
	clk_conf.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&clk_conf) != HAL_OK)
	{
		error_handler();
	}
	
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* GPIO configure */

	/* → PIN_USART_TX */
	gpio_conf.Pin              = PIN_USART_TX.pin;
	gpio_conf.Mode             = GPIO_MODE_AF_PP;
	gpio_conf.Pull             = GPIO_NOPULL;
	gpio_conf.Speed            = GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Alternate        = GPIO_AF7_USART2;
	HAL_GPIO_Init(PIN_USART_TX.port, &gpio_conf);
	
	/* → PIN_USART_RX */
	gpio_conf.Pin              = PIN_USART_RX.pin;
	gpio_conf.Mode             = GPIO_MODE_AF_PP;
	gpio_conf.Pull             = GPIO_NOPULL;
	gpio_conf.Speed            = GPIO_SPEED_FREQ_HIGH;
	gpio_conf.Alternate        = GPIO_AF7_USART2;
	HAL_GPIO_Init(PIN_USART_RX.port, &gpio_conf);

	/* Peripheral configure */
	huart2.Instance            = USART2;
	huart2.Init.BaudRate       = 115200;
	huart2.Init.WordLength     = UART_WORDLENGTH_8B;
	huart2.Init.StopBits       = UART_STOPBITS_1;
	huart2.Init.Parity         = UART_PARITY_NONE;
	huart2.Init.Mode           = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl      = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling   = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;

	huart2.AdvancedInit.AdvFeatureInit      = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
	huart2.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;

	if(HAL_UART_Init(&huart2) != HAL_OK) error_handler();
}

void uart_hello(void)
{
	static char* message = "Hello there!\n";
	HAL_UART_Transmit(&huart2, (uint8_t*)message, 13, 1000);
}
