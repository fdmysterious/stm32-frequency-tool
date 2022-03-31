/* ┌────────────────────┐
   │ Simple PWM control │
   └────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"
#include "pwm.h"

#include <io/gpio.h>
#include <func/error.h>

/* ┌────────────────────────────────────────┐
   │ Data struct                            │
   └────────────────────────────────────────┘ */

struct PWM_Data {
	TIM_HandleTypeDef htim;

	float freq;
	float duty;
};


/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

static struct PWM_Data pwm_data;


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void pwm_init(void)
{
	GPIO_InitTypeDef        gpio_conf          = {0};
	TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig      = {0};
	TIM_OC_InitTypeDef      sConfigOC          = {0};

	/* Clock configure */
    __HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM4_CLK_ENABLE();

	/* GPIO configure */

    gpio_conf.Pin                        = PIN_PWM_OUT.pin;
    gpio_conf.Mode                       = GPIO_MODE_AF_PP;
    gpio_conf.Pull                       = GPIO_NOPULL;
    gpio_conf.Speed                      = GPIO_SPEED_FREQ_LOW;
    gpio_conf.Alternate                  = GPIO_AF2_TIM4;
    HAL_GPIO_Init(PIN_PWM_OUT.port, &gpio_conf);

	/* Timer configure */

	/* → Generic handle */
	pwm_data.htim.Instance               = TIM4;
	pwm_data.htim.Init.Prescaler         = 0;
	pwm_data.htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
	pwm_data.htim.Init.Period            = 16000;
	pwm_data.htim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	pwm_data.htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_Base_Init(&pwm_data.htim) != HAL_OK) error_handler();

	/* Clock source config */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if(HAL_TIM_ConfigClockSource(&pwm_data.htim, &sClockSourceConfig) != HAL_OK) error_handler();

	/* Init PWM */
	if(HAL_TIM_PWM_Init(&pwm_data.htim) != HAL_OK) error_handler();

	/* Trigger config */
	sMasterConfig.MasterOutputTrigger    = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode        = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&pwm_data.htim, &sMasterConfig) != HAL_OK) error_handler();

	/* Output channel config */
	sConfigOC.OCMode                     = TIM_OCMODE_PWM1;
	sConfigOC.Pulse                      = 8000;
	sConfigOC.OCPolarity                 = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode                 = TIM_OCFAST_DISABLE;
	if(HAL_TIM_PWM_ConfigChannel(&pwm_data.htim, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) error_handler();
}

void pwm_start(void)
{
	/* Start TIM4 clock */
	HAL_TIM_PWM_Start(&pwm_data.htim, TIM_CHANNEL_1);
}
