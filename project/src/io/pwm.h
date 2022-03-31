/* ┌────────────────────┐
   │ Simple PWM control │
   └────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#pragma once

void pwm_init    (void);
void pwm_start   (void);

void pwm_freq_set(float f);
void pwm_duty_set(float f);
