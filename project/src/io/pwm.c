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
   │ Config constants                       │
   └────────────────────────────────────────┘ */

#define PWM_TIMER_FREQUENCY 32000000


/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

struct PWM_Data pwm_ch1 = {
	.tim        = TIM4,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM1_OUT,
	.pin_af     = GPIO_AF2_TIM4
};

struct PWM_Data pwm_ch2 = {
	.tim        = TIM2,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM2_OUT,
	.pin_af     = GPIO_AF1_TIM2
};

struct PWM_Data pwm_ch3 = {
	.tim        = TIM1,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM3_OUT,
	.pin_af     = GPIO_AF6_TIM1
};


/* ┌────────────────────────────────────────┐
   │ Private interface                      │
   └────────────────────────────────────────┘ */

static void pwm_update_config(struct PWM_Data *pwm)
{
	TIM_OC_InitTypeDef sConfigOC = {0};
	uint16_t                      duty;
	uint32_t                    period;

	HAL_TIM_PWM_Stop(&pwm->htim, pwm->channel);

	/* Compute period */
	period = (uint32_t)((float)PWM_TIMER_FREQUENCY/pwm->freq);

	/* Compute duty cycle */
	duty = (uint16_t)((float)(period)*pwm->duty);

	/* Update period */
	pwm->htim.Init.Period   = period;
	pwm->htim.Instance->ARR = period;

	/* Update OC config */
	sConfigOC.OCMode        = TIM_OCMODE_PWM1;
	sConfigOC.Pulse         = duty;
	sConfigOC.OCPolarity    = (pwm->polarity == PWM_POSITIVE) ? TIM_OCPOLARITY_HIGH : TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode    = TIM_OCFAST_DISABLE;

	if(HAL_TIM_PWM_ConfigChannel(&pwm->htim, &sConfigOC, pwm->channel) != HAL_OK) error_handler();
	if(pwm->started) HAL_TIM_PWM_Start(&pwm->htim, pwm->channel);
}


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void pwm_init(struct PWM_Data *pwm)
{
	GPIO_InitTypeDef        gpio_conf          = {0};
	TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig      = {0};

	/* Clock configure */
    __HAL_RCC_GPIOB_CLK_ENABLE();

	/* GPIO configure */
    gpio_conf.Pin                        = pwm->pin_output->pin;
    gpio_conf.Mode                       = GPIO_MODE_AF_PP;
    gpio_conf.Pull                       = GPIO_NOPULL;
    gpio_conf.Speed                      = GPIO_SPEED_FREQ_LOW;
    gpio_conf.Alternate                  = pwm->pin_af;
    HAL_GPIO_Init(pwm->pin_output->port, &gpio_conf);

	/* Timer configure */

	/* → Generic handle */
	pwm->htim.Instance               = pwm->tim;
	pwm->htim.Init.Prescaler         = 0;
	pwm->htim.Init.CounterMode       = TIM_COUNTERMODE_UP;
	pwm->htim.Init.Period            = 0;
	pwm->htim.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
	pwm->htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_Base_Init(&pwm->htim) != HAL_OK) error_handler();

	/* Clock source config */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if(HAL_TIM_ConfigClockSource(&pwm->htim, &sClockSourceConfig) != HAL_OK) error_handler();

	/* Init PWM */
	if(HAL_TIM_PWM_Init(&pwm->htim) != HAL_OK) error_handler();

	/* Trigger config */
	sMasterConfig.MasterOutputTrigger    = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode        = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&pwm->htim, &sMasterConfig) != HAL_OK) error_handler();

	/* Initial config */
	pwm->started  = 0;
	pwm->polarity = PWM_POSITIVE;
	pwm->freq     = 2e3f;
	pwm->duty     = 0.5f;

	pwm_update_config(pwm);
}

uint8_t pwm_started_get(struct PWM_Data *pwm)
{
	return pwm->started;
}

void pwm_start(struct PWM_Data *pwm)
{
	pwm->started = 1;
	HAL_TIM_PWM_Start(&pwm->htim, pwm->channel);
}

void pwm_stop(struct PWM_Data *pwm)
{
	pwm->started = 0;
	HAL_TIM_PWM_Stop(&pwm->htim, pwm->channel);
}

void pwm_freq_set(struct PWM_Data *pwm, float f)
{
	if(f < 0.f) return;
	pwm->freq = f;
	pwm_update_config(pwm);
}

void pwm_duty_set(struct PWM_Data *pwm, float f)
{
	if( (f>1.f) || (f < 0.f)) return;

	pwm->duty = f;
	pwm_update_config(pwm);
}

void pwm_polarity_set(struct PWM_Data *pwm, enum PWM_Polarity pol)
{
	if((pol != PWM_POSITIVE) || (pol != PWM_NEGATIVE)) return;

	pwm->polarity = pol;
	pwm_update_config(pwm);
}
