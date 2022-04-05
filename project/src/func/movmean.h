/* ┌────────────────────┐
   │ Simple moving mean │
   └────────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#pragma once

#include <stdint.h>

/* ┌────────────────────────────────────────┐
   │ Datastructures                         │
   └────────────────────────────────────────┘ */

#define MOVING_MEAN_NVALUES 4

struct Moving_Mean_Data {
	int32_t     values[MOVING_MEAN_NVALUES+1];
	int         idx;
	int32_t     mean;
};

/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void  moving_mean_init (struct Moving_Mean_Data *mean);
float moving_mean_push (struct Moving_Mean_Data *mean, int32_t v);
