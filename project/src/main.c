/* ┌────────────────────────────────┐
   │ Simple STM32F3 example program │
   └────────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"

#include <memory.h>

#include <printf/printf.h>

#include <io/clock.h>
#include <io/gpio.h>
#include <io/uart.h>


/* ┌────────────────────────────────────────┐
   │ Main program                           │
   └────────────────────────────────────────┘ */

int main(void)
{
	static char buffer[1024] = {0};

	struct UART_Msg_Info msg = {
		.buffer = NULL,
		.size   = 0
	};

	/* ───────────────── Init ───────────────── */
	

	HAL_Init();

	clock_init();
	gpio_init();
	uart_init();

	/* ─────────────── Main loop ────────────── */

	uart_start();
	
	while(1) {
		do {
			msg = uart_msg_pop();
		} while(msg.buffer == NULL);

		gpio_led_toggle();

		memset  (buffer, 0, 1024);
		snprintf(buffer, 1024, "%.*s\n", msg.size, msg.buffer);

		uart_transmit(buffer, msg.size+1); // +1 for LF char
		msg.buffer = NULL;

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

