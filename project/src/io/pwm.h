/* ┌────────────────────┐
   │ Simple PWM control │
   └────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#pragma once

#include "stm32f3xx_hal.h"

/* ┌────────────────────────────────────────┐
   │ Custom datatypes                       │
   └────────────────────────────────────────┘ */

enum PWM_Polarity {
	PWM_POLARITY_UNKNOWN,
	PWM_POSITIVE,
	PWM_NEGATIVE
};

struct PWM_Data {
	TIM_TypeDef             *tim;
	TIM_HandleTypeDef        htim;
	uint32_t                 channel;

	uint8_t                  started;

	enum PWM_Polarity        polarity;

	float                    freq;
	float                    duty;
	
	const struct GPIO_Pin   *pin_output; /* Pointer to pin definition */
	uint32_t                 pin_af;     /* Pin alternate function */
};



/* ┌────────────────────────────────────────┐
   │ Static instances                       │
   └────────────────────────────────────────┘ */

extern struct PWM_Data pwm_ch1;
//extern struct PWM_Data pwm_ch2;
//extern struct PWM_Data pwm_ch3;


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void    pwm_init        (struct PWM_Data *pwm);

uint8_t pwm_started_get (struct PWM_Data *pwm);
void    pwm_start       (struct PWM_Data *pwm);
void    pwm_stop        (struct PWM_Data *pwm);

void    pwm_freq_set    (struct PWM_Data *pwm, float f              );
void    pwm_duty_set    (struct PWM_Data *pwm, float d              );
void    pwm_polarity_set(struct PWM_Data *pwm, enum PWM_Polarity pol);
