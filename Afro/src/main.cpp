//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "BlinkLed.h"
#include <os_dbg.h>
#include <core_cmInstr.h>
#include <uart.h>
#include <string.h>

#include <_boot.h>

// ----------------------------------------------------------------------------
//
// Standalone STM32F1 led blink sample (trace via NONE).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8 MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f10x.c
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//to program run commands:
//sudo stm32flash -w ./MBS_pilot_blinky.hex -v -g 0x0 /dev/ttyUSB0
//sudo stm32flash -w ./MBS_pilot_blinky.hex -v -c -g 0x0 /dev/ttyUSB0
//
//The first will fail but will configure the device to use the correct
//  uart device and baud rate and the second with complete the task
//  of programming the chip

int kernel_main(void*);


BlinkLed blinkLed;

int
main(int argc, char* argv[])
{
	blinkLed.powerUp();
	//return kernel_main(NULL);
	//Uart* uart2 = Uart::GetUartIsrSafe(Uart::GPS);
	//uart2->WriteIsrSafe((u8*)"Test\r\n", 6);
	//OS::Init();
	os_dbg::Init();
	for (int cnt = 0; cnt < 30; cnt++) {
		blinkLed.statusOn();
		volatile int i = 2000000;
		while(i--);
		blinkLed.statusOff();
		i = 1000000;
		while(i--) SCB->ICSR = SCB_ICSR_PENDSVSET;
	}

	_boot_load();
}

int kernel_main(void* args) {

	// Perform all necessary initialisations for the LED.

	//if ((RCC->CFGR & RCC_CFGR_PLLXTPRE) != 0) {
	//	blinkLed.statusOn();
	//} else {
	//	blinkLed.statusOff();
	//}

	uint32_t seconds = 0;

	u32 ticksPsecond = 10000;

	Uart* uart1 = Uart::GetUart(Uart::USB);
	Uart* uart2 = Uart::GetUartIsrSafe(Uart::GPS);

	char read_value = 0;
	while(read_value != 'q'){
		read_value = uart1->ReadByte(0);
		uart1->WriteByte(read_value);
		uart2->WriteHex(read_value,2,true);

	}



	// Infinite loop
	int cnt = 30;
	while (cnt--)
	{
	  blinkLed.statusOn();
	  //blinkLed.turnOn();
	  u32 next = OS::SystemTicks() + ticksPsecond / 2;
	  while ((int)(next - OS::SystemTicks()) > 0){}

	  //blinkLed.turnOff();
      blinkLed.statusOff();
	  next = OS::SystemTicks() + ticksPsecond / 2;
	  while ((int)(next - OS::SystemTicks()) > 0){}
	  char * writeUart1 = (char*)"Here I am writing things for Uart1\r\n";
	  char * writeUart2 = (char*)"Here I am writing things for Uart2\r\n";
	  char * writeUart3 = (char*)"Here I am writing things for Uart3\r\n";


	  uart1->WriteBytes(writeUart1,strlen(writeUart1));
	  uart2->WriteIsrSafe(writeUart2,strlen(writeUart2));

	  ++seconds;

	  // Count seconds on the trace device.
	  trace_printf("Second %u\n", seconds);
	}



	blinkLed.turnOn();
	u32 next = OS::SystemTicks() + ticksPsecond * 4;
	while ((int)(next - OS::SystemTicks()) > 0){}

	_boot_load();
	// should never reach here
}

extern "C" void HardFault_Handler() {
	while(1) {
	  blinkLed.turnOn();
	  volatile int i = 1000000;
	  while(i--){}
	  blinkLed.turnOff();
	  i = 1000000;
	  while(i--){}
	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
