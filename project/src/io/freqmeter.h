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
   │ Constantes                             │
   └────────────────────────────────────────┘ */
#define FREQMETER_DIVIDE_FACTOR 32 // Timer clock frequency is 32 MHz, and we want microseconds as result, so 32e6 * 1e-6 = 32


/* ┌────────────────────────────────────────┐
   │ Datatypes                              │
   └────────────────────────────────────────┘ */

enum Freqmeter_State {
	FREQMETER_STATE_START,    // Waiting for first edge
	FREQMETER_STATE_POSITIVE, // Measuring positive time
	FREQMETER_STATE_NEGATIVE  // Measuring negative time
};

struct Freqmeter_Data {
	TIM_TypeDef                *tim;
	TIM_HandleTypeDef           htim;
	uint32_t                    channel;

	enum Freqmeter_State        state;

	const struct GPIO_Pin      *pin_input;
	uint32_t                    pin_af;

	struct Moving_Mean_Data     movmean_positive;
	struct Moving_Mean_Data     movmean_negative;

	float                       last_positive;
	float                       last_negative;

	uint32_t                    irq;

	int32_t last;
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
//extern struct Freqmeter_Data fmeter2;
//extern struct Freqmeter_Data fmeter3;
