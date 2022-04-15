/* ┌────────────────────────┐
   │ Simple frequency meter │
   └────────────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#include "stm32f3xx_hal.h"
#include "freqmeter.h"

#include <func/error.h>


/* ┌────────────────────────────────────────┐
   │ Forward declarations                   │
   └────────────────────────────────────────┘ */

void freqmeter_IC_callback (TIM_HandleTypeDef *htim);


/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */

struct Freqmeter_Data fmeter1 = {
	.tim         = TIM2,
	.channel     = TIM_CHANNEL_1,
	.pin_input   = &PIN_FMETER1_IN,
	.pin_af      = GPIO_AF1_TIM2,
	.irq         = TIM2_IRQn
};

struct Freqmeter_Data fmeter2 = {
	.tim         = TIM3,
	.channel     = TIM_CHANNEL_2,
	.pin_input   = &PIN_FMETER2_IN,
	.pin_af      = GPIO_AF2_TIM3,
	.irq         = TIM3_IRQn
};

struct Freqmeter_Data fmeter3 = {
	.tim         = TIM4,
	.channel     = TIM_CHANNEL_1,
	.pin_input   = &PIN_FMETER3_IN,
	.pin_af      = GPIO_AF2_TIM4,
	.irq         = TIM4_IRQn
};

struct Freqmeter_Data* __fmeter_from_htim(TIM_HandleTypeDef *htim)
{
	if     (htim == &fmeter1.htim) return &fmeter1;
	else if(htim == &fmeter2.htim) return &fmeter2;
	else if(htim == &fmeter3.htim) return &fmeter3;
	else                           return  NULL;
}


/* ┌────────────────────────────────────────┐
   │ Private interface                      │
   └────────────────────────────────────────┘ */

void __freqmeter_config_update(struct Freqmeter_Data *fmeter)
{
	TIM_IC_InitTypeDef sConfigIC = {0};

	/* Configure Input Capture */
	sConfigIC.ICPolarity  = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter    = 4;
	if(HAL_TIM_IC_ConfigChannel(&fmeter->htim, &sConfigIC, fmeter->channel) != HAL_OK) error_handler();

}

uint8_t __freqmeter_complementary_channel(struct Freqmeter_Data *fmeter)
{
	if(fmeter->channel == TIM_CHANNEL_1) return TIM_CHANNEL_2;
	else                                 return TIM_CHANNEL_1;
}


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void freqmeter_init(struct Freqmeter_Data *fmeter)
{
	TIM_IC_InitTypeDef      sConfigIC          = {0};
	TIM_SlaveConfigTypeDef  sSlaveConfig       = {0};
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

	/* Configure main input channel */
	sConfigIC.ICPolarity                       = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection                      = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler                      = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter                         = 4;
	if(HAL_TIM_IC_ConfigChannel(&fmeter->htim, &sConfigIC, fmeter->channel) != HAL_OK) error_handler();

	/* Configure secondary input channel */
	sConfigIC.ICPolarity                       = TIM_INPUTCHANNELPOLARITY_FALLING;
	sConfigIC.ICSelection                      = TIM_ICSELECTION_INDIRECTTI;
	sConfigIC.ICPrescaler                      = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter                         = 4;
	if(HAL_TIM_IC_ConfigChannel(&fmeter->htim, &sConfigIC, __freqmeter_complementary_channel(fmeter)) != HAL_OK) error_handler();
	
	/* Slave reset config */
	sSlaveConfig.SlaveMode    = TIM_SLAVEMODE_RESET;
	sSlaveConfig.InputTrigger = fmeter->channel == TIM_CHANNEL_1 
		? TIM_TS_TI1FP1
		: TIM_TS_TI2FP2
	;
	sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
	sSlaveConfig.TriggerPrescaler= TIM_TRIGGERPRESCALER_DIV1;
	sSlaveConfig.TriggerFilter   = 0;
	if(HAL_TIM_SlaveConfigSynchro(&fmeter->htim, &sSlaveConfig) != HAL_OK) error_handler();
	

	/* GPIO Init */
    gpio_conf.Pin                              = fmeter->pin_input->pin;
    gpio_conf.Mode                             = GPIO_MODE_AF_PP;
    gpio_conf.Pull                             = GPIO_NOPULL;
    gpio_conf.Speed                            = GPIO_SPEED_FREQ_LOW;
    gpio_conf.Alternate                        = fmeter->pin_af;
    HAL_GPIO_Init(fmeter->pin_input->port, &gpio_conf);


	/* Movmean init */
	moving_mean_init(&fmeter->movmean_period);
	moving_mean_init(&fmeter->movmean_positive);


	/* Enable interrupt */
	HAL_NVIC_SetPriority(fmeter->irq, 0, 0);
	HAL_NVIC_EnableIRQ  (fmeter->irq);
}

void freqmeter_start(struct Freqmeter_Data *fmeter)
{
	HAL_TIM_IC_Start_IT(&fmeter->htim, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&fmeter->htim, TIM_CHANNEL_2);
}

void freqmeter_stop(struct Freqmeter_Data *fmeter)
{
	HAL_TIM_IC_Stop_IT(&fmeter->htim, TIM_CHANNEL_1);
	HAL_TIM_IC_Stop_IT(&fmeter->htim, TIM_CHANNEL_2);
}

/* ┌────────────────────────────────────────┐
   │ IRQ callbacks                          │
   └────────────────────────────────────────┘ */

/* Input capture callback */

void freqmeter_IC_callback(TIM_HandleTypeDef *htim) 
{
	uint8_t comp_channel;
	uint8_t active_channel;

	int32_t capture_period;
	int32_t capture_duty;

	struct Freqmeter_Data *fmeter;

	/* Find correct fmeter instance */
	fmeter = __fmeter_from_htim(htim);
	if(fmeter == NULL) return;

	comp_channel   = __freqmeter_complementary_channel(fmeter);
	active_channel = (fmeter->channel == TIM_CHANNEL_1)
		? HAL_TIM_ACTIVE_CHANNEL_1
		: HAL_TIM_ACTIVE_CHANNEL_2
	;

	if(fmeter->htim.Channel == active_channel) {
		capture_period   = HAL_TIM_ReadCapturedValue(htim, fmeter->channel);
		capture_duty     = HAL_TIM_ReadCapturedValue(htim, comp_channel   );

		moving_mean_push(&fmeter->movmean_period, capture_period );
		moving_mean_push(&fmeter->movmean_positive, capture_duty );
	}
}


/* ┌────────────────────────────────────────┐
   │ IRQHandlers                            │
   └────────────────────────────────────────┘ */

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&fmeter1.htim);
}

void TIM3_IRQHandler(void)
{
	gpio_led_toggle();
	HAL_TIM_IRQHandler(&fmeter2.htim);
}

void TIM4_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&fmeter3.htim);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	freqmeter_IC_callback(htim);
}
