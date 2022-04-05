/* ┌────────────────────────┐
   │ Simple frequency meter │
   └────────────────────────┘
   
    Florian Dupeyron
    April 2022

	This interfaces uses raw interrupt flags instead of HAL stuff
	for Input capture channel.
*/

#include "stm32f3xx_hal.h"
#include "freqmeter.h"

#include <func/error.h>


/* ┌────────────────────────────────────────┐
   │ Forward declarations                   │
   └────────────────────────────────────────┘ */

void freqmeter_OVF_callback(TIM_HandleTypeDef *htim);
void freqmeter_IC_callback (TIM_HandleTypeDef *htim);
/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

struct Freqmeter_Data fmeter1 = {
	.tim         = TIM15,
	.channel     = TIM_CHANNEL_1,
	.pin_input   = &PIN_FMETER1_IN,
	.pin_af      = GPIO_AF1_TIM15,
	.irq         = TIM1_BRK_TIM15_IRQn
};

struct Freqmeter_Data* __fmeter_from_htim(TIM_HandleTypeDef *htim)
{
	if(htim == &fmeter1.htim) return &fmeter1;
	else                      return    NULL;
}


/* ┌────────────────────────────────────────┐
   │ Private interface                      │
   └────────────────────────────────────────┘ */

void __freqmeter_config_update(struct Freqmeter_Data *fmeter)
{
	/* Disable interrupt */
	TIM_IC_InitTypeDef sConfigIC = {0};

	/* Configure Input Capture */
	sConfigIC.ICPolarity  = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter    = 0;
	if(HAL_TIM_IC_ConfigChannel(&fmeter->htim, &sConfigIC, fmeter->channel) != HAL_OK) error_handler();

	/* Enable interrupt */
	HAL_NVIC_SetPriority(fmeter->irq, 0, 0);
	HAL_NVIC_EnableIRQ  (fmeter->irq);
}


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void freqmeter_init(struct Freqmeter_Data *fmeter)
{
	TIM_MasterConfigTypeDef sMasterConfig      = {0};
	GPIO_InitTypeDef        gpio_conf          = {0};

	/* HAL handler init */
	fmeter->htim.Instance                      = fmeter->tim;
	fmeter->htim.Init.Prescaler                = 0;
	fmeter->htim.Init.CounterMode              = TIM_COUNTERMODE_UP;
	fmeter->htim.Init.Period                   = 65535;
	fmeter->htim.Init.ClockDivision            = TIM_CLOCKDIVISION_DIV1;
	fmeter->htim.Init.RepetitionCounter        = 0;
	fmeter->htim.Init.AutoReloadPreload        = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if(HAL_TIM_IC_Init(&fmeter->htim) != HAL_OK) error_handler();

	/* Master trigger config */
	sMasterConfig.MasterOutputTrigger          = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode              = TIM_MASTERSLAVEMODE_DISABLE;
	if(HAL_TIMEx_MasterConfigSynchronization(&fmeter->htim, &sMasterConfig) != HAL_OK) error_handler();

	/* GPIO Init */
    gpio_conf.Pin                              = fmeter->pin_input->pin;
    gpio_conf.Mode                             = GPIO_MODE_AF_PP;
    gpio_conf.Pull                             = GPIO_NOPULL;
    gpio_conf.Speed                            = GPIO_SPEED_FREQ_LOW;
    gpio_conf.Alternate                        = fmeter->pin_af;
    HAL_GPIO_Init(fmeter->pin_input->port, &gpio_conf);

	/* Movmean init */
	moving_mean_init(&fmeter->movmean_negative);
	moving_mean_init(&fmeter->movmean_positive);

	fmeter->last_negative = 0.f;
	fmeter->last_positive = 0.f;

	/* State init */
	fmeter->state = FREQMETER_STATE_START;

	/* Configure input channel and interrupt */
	__freqmeter_config_update(fmeter);
}

void freqmeter_start(struct Freqmeter_Data *fmeter)
{
	HAL_TIM_IC_Start_IT(&fmeter->htim, fmeter->channel);
}

void freqmeter_stop(struct Freqmeter_Data *fmeter)
{
	HAL_TIM_IC_Stop_IT(&fmeter->htim, fmeter->channel);
}

/* ┌────────────────────────────────────────┐
   │ IRQ callbacks                          │
   └────────────────────────────────────────┘ */

/* Period overflow callback */
/* The value is reset and the last known value is saved	*/

void freqmeter_OVF_callback(TIM_HandleTypeDef *htim)
{
	struct Freqmeter_Data *fmeter = __fmeter_from_htim(htim);
	if(fmeter == NULL) return;

	/* Save last known values, reset all stuff */
	fmeter->last_negative = fmeter->movmean_negative.mean;
	fmeter->last_positive = fmeter->movmean_positive.mean;

	moving_mean_init(&fmeter->movmean_positive);
	moving_mean_init(&fmeter->movmean_negative);

	fmeter->state = FREQMETER_STATE_START;
}

/* Input capture callback */

void freqmeter_IC_callback(TIM_HandleTypeDef *htim) 
{
	int32_t capture;
	int32_t diff;
	struct Freqmeter_Data *fmeter;

	/* Find correct fmeter instance */
	fmeter = __fmeter_from_htim(htim);
	if(fmeter == NULL) return;

	capture = __HAL_TIM_GET_COMPARE(&fmeter->htim, fmeter->channel);

	/* Get capture value */
	diff    = (fmeter->last > capture) ? (1<<16) : 0;
	diff   += capture;
	diff   -= fmeter->last;

	fmeter->last = capture;


	/* Process value from edge and current state */
	switch(fmeter->state) {
		default:
		case FREQMETER_STATE_START:
			/* Rising edge */
			if(gpio_pin_get(*fmeter->pin_input)) {
				fmeter->state = FREQMETER_STATE_POSITIVE;
				moving_mean_push(&fmeter->movmean_positive, diff);
			}

			/* Falling edge */
			else {
				fmeter->state = FREQMETER_STATE_NEGATIVE;
				moving_mean_push(&fmeter->movmean_negative, diff);
			}
			break;

		case FREQMETER_STATE_POSITIVE:
			/* Was falling edge */
			if(!gpio_pin_get(*fmeter->pin_input)) {
				moving_mean_push(&fmeter->movmean_positive, diff);
				fmeter->state = FREQMETER_STATE_NEGATIVE;
			}
			break;

		case FREQMETER_STATE_NEGATIVE:
			/* Was rising edge */
			if(gpio_pin_get(*fmeter->pin_input)) {
				moving_mean_push(&fmeter->movmean_negative, diff);
				fmeter->state = FREQMETER_STATE_POSITIVE;
			}
			break;
	}
}


/* ┌────────────────────────────────────────┐
   │ IRQHandlers                            │
   └────────────────────────────────────────┘ */

void TIM1_BRK_TIM15_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&fmeter1.htim);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	freqmeter_IC_callback(htim);
}
