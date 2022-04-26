/* ┌────────────────────┐
   │ Simple PWM control │
   └────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"
#include "pwm.h"

#include <math.h>

#include <func/error.h>
#include <bsp/pin.h>


/* ┌────────────────────────────────────────┐
   │ Config constants                       │
   └────────────────────────────────────────┘ */

#define PWM_TIMER_FREQUENCY HAL_RCC_GetHCLKFreq()


/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

struct PWM_Data pwm_ch1 = {
	.tim        = TIM15,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM1_OUT,
	.pin_af     = GPIO_AF1_TIM15
};

struct PWM_Data pwm_ch2 = {
	.tim        = TIM16,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM2_OUT,
	.pin_af     = GPIO_AF1_TIM16
};

struct PWM_Data pwm_ch3 = {
	.tim        = TIM17,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM3_OUT,
	.pin_af     = GPIO_AF1_TIM17
};

struct PWM_Data pwm_ch4 = {
	.tim        = TIM1,
	.channel    = TIM_CHANNEL_1,
	.pin_output = &PIN_PWM4_OUT,
	.pin_af     = GPIO_AF6_TIM1
};


/* ┌────────────────────────────────────────┐
   │ Private interface                      │
   └────────────────────────────────────────┘ */

static void pwm_update_config(struct PWM_Data *pwm)
{
	/* Period and prescaler are calculated using a simple
	 * O(1) algorithm, where the minimum prescaler to have
	 * period <= 65535 is found, then the period is computed
	 * properly. This simple algorithm gives overall good results,
	 * with 0.16% error @ 100 kHz, and < 0.01% error at 10kHz.
	 * This algorithm also minimizes the error for the duty cycle.*/
	
	TIM_OC_InitTypeDef  sConfigOC = {0};

	uint32_t            clk       = PWM_TIMER_FREQUENCY;

	float               prescaler = 0;
	float               period    = 0;

	uint32_t            duty      = 0;

	HAL_TIM_PWM_Stop(&pwm->htim, pwm->channel);

	/* Compute min. prescaler and period */
	prescaler = ceilf((float)clk/( ((float)(1<<16)) * pwm->freq ));
	period    = ((float)clk)/(prescaler*pwm->freq);

	/* Compute duty cycle */
	duty      = (uint16_t)(period*pwm->duty);

	/* Update period and prescaler in config */
	pwm->htim.Init.Period     = (uint32_t)period;
	pwm->htim.Init.Prescaler  = (uint32_t)(prescaler - 1.f);
	pwm->htim.Instance->ARR   = (uint32_t)period;
	pwm->htim.Instance->PSC   = (uint32_t)(prescaler - 1.f);

	/* Generate an update event to reload the Prescaler
	   and the repetition counter (only for advanced timer) value immediately */
	pwm->htim.Instance->EGR = TIM_EGR_UG;

	/* Update OC config */
	sConfigOC.OCMode          = TIM_OCMODE_PWM1;
	sConfigOC.Pulse           = duty;
	sConfigOC.OCPolarity      = (pwm->polarity == PWM_POSITIVE) ? TIM_OCPOLARITY_HIGH : TIM_OCPOLARITY_LOW;
	sConfigOC.OCFastMode      = TIM_OCFAST_DISABLE;

	if(HAL_TIM_PWM_ConfigChannel(&pwm->htim, &sConfigOC, pwm->channel) != HAL_OK) error_handler();

	/* Restart PWM if needed */
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

	/* GPIO configure */
    gpio_conf.Pin                     = pwm->pin_output->pin;
    gpio_conf.Mode                    = GPIO_MODE_AF_PP;
    gpio_conf.Pull                    = GPIO_NOPULL;
    gpio_conf.Speed                   = GPIO_SPEED_FREQ_LOW;
    gpio_conf.Alternate               = pwm->pin_af;
    HAL_GPIO_Init(pwm->pin_output->port, &gpio_conf);

	/* Timer configure */

	/* → Generic handle */
	pwm->htim.Instance                = pwm->tim;
	pwm->htim.Init.Prescaler          = 0;
	pwm->htim.Init.CounterMode        = TIM_COUNTERMODE_UP;
	pwm->htim.Init.Period             = 0;
	pwm->htim.Init.ClockDivision      = TIM_CLOCKDIVISION_DIV1;
	pwm->htim.Init.AutoReloadPreload  = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_Base_Init(&pwm->htim) != HAL_OK) error_handler();

	/* Clock source config */
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if(HAL_TIM_ConfigClockSource(&pwm->htim, &sClockSourceConfig) != HAL_OK) error_handler();

	/* Init PWM */
	if(HAL_TIM_PWM_Init(&pwm->htim) != HAL_OK) error_handler();

	/* Trigger config */
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
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
