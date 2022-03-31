/* ┌────────────────────┐
   │ Simple PWM control │
   └────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#pragma once

/* ┌────────────────────────────────────────┐
   │ Custom datatypes                       │
   └────────────────────────────────────────┘ */

enum PWM_Polarity {
	PWM_POLARITY_UNKNOWN,
	PWM_POSITIVE,
	PWM_NEGATIVE
};


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void    pwm_init        (void);

uint8_t pwm_started_get (void);
void    pwm_start       (void);
void    pwm_stop        (void);

void    pwm_freq_set    (float f);
void    pwm_duty_set    (float f);
void    pwm_polarity_set(enum PWM_Polarity pol);
