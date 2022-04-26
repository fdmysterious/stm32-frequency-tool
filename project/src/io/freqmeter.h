/* ┌────────────────────────┐
   │ Simple frequency meter │
   └────────────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#pragma once

#include "stm32f3xx_hal.h"

#include <func/movmean.h>
#include <io/gpio.h>


/* ┌────────────────────────────────────────┐
   │ Datatypes                              │
   └────────────────────────────────────────┘ */

struct Freqmeter_Data {
	TIM_TypeDef                *tim;
	TIM_HandleTypeDef           htim;
	uint32_t                    channel;
	uint32_t                    irq;

	const struct Pin           *pin_input;
	uint32_t                    pin_af;

	struct Moving_Mean_Data     movmean_period;
	struct Moving_Mean_Data     movmean_positive;
};


/* ┌────────────────────────────────────────┐
   │ Public interfaces                      │
   └────────────────────────────────────────┘ */

void freqmeter_init (struct Freqmeter_Data *fmeter);
void freqmeter_start(struct Freqmeter_Data *fmeter);
void freqmeter_stop (struct Freqmeter_Data *fmeter);


/* ┌────────────────────────────────────────┐
   │ Instances                              │
   └────────────────────────────────────────┘ */

extern struct Freqmeter_Data fmeter1;
extern struct Freqmeter_Data fmeter2;
extern struct Freqmeter_Data fmeter3;
