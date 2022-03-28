/* ┌────────────────────────────────┐
   │ Simple STM32F3 example program │
   └────────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"

#include "io/clock.h"
#include "io/gpio.h"


/* ┌────────────────────────────────────────┐
   │ Main program                           │
   └────────────────────────────────────────┘ */

int main(void)
{
	HAL_Init();

	clock_init();
	gpio_init();

	while(1) {
		gpio_pin_set(PIN_LD2, 1);
		HAL_Delay(1000);
		gpio_pin_set(PIN_LD2, 0);
		HAL_Delay(1000);
	}
}


/* ┌────────────────────────────────────────┐
   │ Main interrupts definition             │
   └────────────────────────────────────────┘ */

/* ─────────── System interrupts ────────── */

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
	HAL_IncTick();
}


/* ─────────── Fault interrupts ─────────── */


void HardFault_Handler(void)
{
	while(1);
}

void MemManage_Handler(void)
{
	while(1);
}

void BusFault_Handler(void)
{
	while(1);
}

void UsageFault_Handler(void)
{
	while(1);
}

