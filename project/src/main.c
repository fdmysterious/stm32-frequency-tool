/* ┌────────────────────────────────┐
   │ Simple STM32F3 example program │
   └────────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"

#include <io/clock.h>
#include <io/gpio.h>
#include <io/uart.h>


/* ┌────────────────────────────────────────┐
   │ Main program                           │
   └────────────────────────────────────────┘ */

int main(void)
{
	char lf = '\n';

	struct UART_Msg_Info msg = {
		.buffer = NULL,
		.size   = 0
	};

	uint8_t ld_state = 0;
	HAL_Init();

	clock_init();
	gpio_init();
	uart_init();

	uart_start();
	while(1) {
		do {
			msg = uart_msg_pop();
		} while(msg.buffer == NULL);

		gpio_led_toggle();

		uart_transmit(msg.buffer, msg.size);
		msg.buffer = NULL;

		while(!uart_transmit_done());
		uart_transmit(&lf, 1);
		while(!uart_transmit_done());
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
	while(1) {
		gpio_led_toggle();
		HAL_Delay(100);
	}
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

