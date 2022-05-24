/* ┌─────────────────────────┐
   │ Analog comparator stuff │
   └─────────────────────────┘
   
    Florian Dupeyron
    April 2022
*/

#include "stm32f3xx_hal.h"
#include "acomp.h"

/* TODO : Move this */

struct Analog_Comparator acomp = {
	.comp      = COMP1,
	.pin_input = &PIN_ACOMP_IN,
	.polarity  = ACOMP_POLARITY_STANDARD,
	.ref       = ACOMP_REF_VREF_DIV4
};

/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void acomp_init(struct Analog_Comparator *acomp)
{
	/* Init peripheral */
	acomp->hcomp.Init.NonInvertingInput = COMP_NONINVERTINGINPUT_IO1;
	acomp->hcomp.Init.Output            = COMP_OUTPUT_NONE; /* Not connected to another peripheral */
	acomp->hcomp.Init.Hysteresis        = COMP_HYSTERESIS_NONE;
	acomp->hcomp.Init.BlankingSrce      = COMP_BLANKINGSRCE_NONE;
	acomp->hcomp.Init.Mode              = COMP_MODE_HIGHSPEED;
	acomp->hcomp.Init.WindowMode        = COMP_WINDOWMODE_DISABLE;
	acomp->hcomp.Init.TriggerMode       = COMP_TRIGGERMODE_NONE;


	switch(acomp->polarity) {
		case ACOMP_POLARITY_STANDARD:
			acomp->hcomp.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
			break;

		case ACOMP_POLARITY_INVERTED:
			acomp->hcomp.Init.OutputPol = COMP_OUTPUTPOL_INVERTED;
			break;
	}

	switch(acomp->ref) {
		case ACOMP_REF_VREF_DIV4:
			acomp->hcomp.Init.InvertingInput = COMP_INVERTINGINPUT_1_4VREFINT;
			break;

		case ACOMP_REF_VREF_DIV2:
			acomp->hcomp.Init.InvertingInput = COMP_INVERTINGINPUT_1_2VREFINT;
			break;

		case ACOMP_REF_VREF_3DIV4:
			acomp->hcomp.Init.InvertingInput = COMP_INVERTINGINPUT_3_4VREFINT;
			break;

		case ACOMP_REF_VREF:
			acomp->hcomp.Init.InvertingInput = COMP_INVERTINGINPUT_VREFINT;
			break;
	}

	acomp->hcomp.Instance = acomp->comp;

	if(HAL_COMP_Init(&acomp->hcomp) != HAL_OK) {
		error_handler();
	}

	/* Init GPIO */
	GPIO_InitTypeDef gpio_init = {
		.Pin       = acomp->pin_input->pin,
		.Mode      = GPIO_MODE_ANALOG,
		.Pull      = GPIO_NOPULL
	};

	HAL_GPIO_Init(acomp->pin_input->port, &gpio_init);
}

void acomp_enable(struct Analog_Comparator *acomp)
{
	HAL_COMP_Start(&acomp->hcomp);
}

void acomp_disable(struct Analog_Comparator *acomp)
{
	HAL_COMP_Stop(&acomp->hcomp);
}

uint8_t acomp_value_get(struct Analog_Comparator *acomp)
{
	return HAL_COMP_GetOutputLevel(&acomp->hcomp) == COMP_OUTPUTLEVEL_HIGH
		? 1
		: 0
	;
}
