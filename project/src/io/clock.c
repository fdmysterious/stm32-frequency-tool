/* ┌─────────────────┐
   │ Clock managment │
   └─────────────────┘

   Florian Dupeyron
   March 2022
   */

#include "clock.h"
#include "stm32f3xx_hal.h"

#include <func/error.h>

void clock_init(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct   = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0};
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState             = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue       = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState             = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState         = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource        = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL           = RCC_PLL_MUL4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) error_handler();

	/** Initializes the CPU, AHB and APB buses clocks
	*/
	RCC_ClkInitStruct.ClockType = \
		  RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		| RCC_CLOCKTYPE_PCLK1
		| RCC_CLOCKTYPE_PCLK2
	;
	RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider  = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) error_handler();

	/* Peripheral clocks */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	__HAL_RCC_TIM1_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_RCC_TIM15_CLK_ENABLE();

	PeriphClkInit.PeriphClockSelection             = RCC_PERIPHCLK_TIM1;
	PeriphClkInit.Tim1ClockSelection               = RCC_TIM1CLK_HCLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) error_handler();
}
