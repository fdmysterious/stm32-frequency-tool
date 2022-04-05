/* ┌────────────────────┐
   │ Simple moving mean │ └────────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#include "movmean.h"


void moving_mean_init(struct Moving_Mean_Data *mean)
{
	int i = MOVING_MEAN_NVALUES+1;
	while(i--) {
		mean->values[i] = 0;
	}

	mean->idx  = 0;
	mean->mean = 0;
}


float moving_mean_push(struct Moving_Mean_Data *mean, int32_t v)
{
	/* Find n-N index */
	int idx_p = mean->idx - MOVING_MEAN_NVALUES;
	if(idx_p < 0) idx_p  += MOVING_MEAN_NVALUES+1; // Wrap around
	
	/* Update sum */
	int32_t update = v - mean->values[idx_p];

	/* Store value and update index */
	mean->values[mean->idx]  = v;
	mean->idx++;
	mean->idx %= (MOVING_MEAN_NVALUES+1);

	/* Update mean value */
	mean->mean += update / MOVING_MEAN_NVALUES;
	return mean->mean;
}
