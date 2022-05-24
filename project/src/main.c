/* ┌────────────────────────────────┐
   │ Simple STM32F3 example program │
   └────────────────────────────────┘
   
    Florian Dupeyron
    March 2022
*/

#include "stm32f3xx_hal.h"

#include <memory.h>

#include <printf/printf.h>

#include <bsp/pin.h>

#include <io/clock.h>
#include <io/gpio.h>
#include <io/uart.h>
#include <io/pwm.h>
#include <io/freqmeter.h>
#include <io/acomp.h>


#include <func/cmds.h>

/* ┌────────────────────────────────────────┐
   │ Static data                            │
   └────────────────────────────────────────┘ */



/* ┌────────────────────────────────────────┐
   │ Main program                           │
   └────────────────────────────────────────┘ */

int main(void)
{
	static char buffer[1024] = {0};
	size_t r_len = 0;

	struct UART_Msg_Info msg = {
		.buffer = NULL,
		.size   = 0
	};

	/* ───────────────── Init ───────────────── */
	

	HAL_Init();

	clock_init();
	gpio_init();
	uart_init();

	pwm_init      (&pwm_ch1);
	pwm_init      (&pwm_ch2);
	pwm_init      (&pwm_ch3);
	pwm_init      (&pwm_ch4);

	freqmeter_init(&fmeter1);
	freqmeter_init(&fmeter2);
	freqmeter_init(&fmeter3);

	acomp_init(&acomp);

	cmds_init();

	/* ─────────────── Main loop ────────────── */

	uart_start();
	freqmeter_start(&fmeter1);
	freqmeter_start(&fmeter2);
	freqmeter_start(&fmeter3);

	acomp_enable(&acomp);

	while(1) {
		do {
			msg = uart_msg_pop();
		} while(msg.buffer == NULL);

		//gpio_led_toggle();
		r_len = prpc_process_line(msg.buffer, buffer, 1023); // Keep at least one char for LF

		if(r_len) { // if a response has been processed, r_len > 0
			buffer[r_len] = '\n';
			uart_transmit(buffer, r_len+1); // +1 for LF char
			msg.buffer = NULL;

			while(!uart_transmit_done());
		}

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

