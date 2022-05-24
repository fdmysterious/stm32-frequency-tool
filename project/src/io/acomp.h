/* ┌──────────────────────────────────┐
   │ Simple analog comparator handler │
   └──────────────────────────────────┘
   
    Florian Dupeyron
    April 2022

	This driver stays really basic and doesn't implement :
		-> Output comparator selection
		-> Hysteresis
		-> Blanking
		-> Other input sources than Vrefint
*/

#pragma once

#include <inttypes.h>
#include "stm32f3xx.h"

#include <bsp/pin.h>


/* ┌────────────────────────────────────────┐
   │ Enums and struct                       │
   └────────────────────────────────────────┘ */


enum Analog_Comparator_Polarity {
	ACOMP_POLARITY_STANDARD = 0x00,
	ACOMP_POLARITY_INVERTED = 0x01
};

enum Analog_Comparator_Ref {
	ACOMP_REF_VREF_DIV4  = 0x00, /* For 3.3V: 0.825V */
	ACOMP_REF_VREF_DIV2  = 0x01, /* For 3.3V: 1.65V  */
	ACOMP_REF_VREF_3DIV4 = 0x02, /* For 3.3V: 2.47V  */
	ACOMP_REF_VREF       = 0x03  /* For 3.3V: 3.3V   */
};

struct Analog_Comparator {
	/* ──────────── Hardware handle ─────────── */
	
	COMP_TypeDef                   *comp;
	COMP_HandleTypeDef             hcomp;


	/* ─────────────── Pin def. ─────────────── */

	const struct Pin               *pin_input;


	/* ───────────── Init. Options ──────────── */
	
	enum Analog_Comparator_Polarity polarity;
	enum Analog_Comparator_Ref           ref;
};


/* ┌────────────────────────────────────────┐
   │ Public interface                       │
   └────────────────────────────────────────┘ */

void    acomp_init     (struct Analog_Comparator *acomp);

void    acomp_enable   (struct Analog_Comparator *acomp);
void    acomp_disable  (struct Analog_Comparator *acomp);

uint8_t acomp_value_get(struct Analog_Comparator *acomp);

/* TODO : Move this */
extern struct Analog_Comparator acomp;
