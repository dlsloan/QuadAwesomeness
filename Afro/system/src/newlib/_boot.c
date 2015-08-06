#include <stm32f10x.h>
#include <_boot.h>

#define BOOTLOADER_ENTRY (0x1FFFF000)

#define BOOTLOAD_ENABLE  (0xad3ef65c)
#define BOOTLOAD_RESET   (0x84936650)

unsigned int bootLoad;

typedef  void (*bootFunc)(void);

void _boot_switch() {
	if (bootLoad == BOOTLOAD_ENABLE) {
		bootLoad = BOOTLOAD_RESET;
		bootFunc booter = (bootFunc)
				(((volatile unsigned int*)(BOOTLOADER_ENTRY))[1]);
		booter();
	} else if (bootLoad == BOOTLOAD_RESET) {
		bootLoad = 0;
		__disable_irq();
		NVIC_SystemReset();
	}
}

void _boot_load() {
	bootLoad = BOOTLOAD_ENABLE;
	__disable_irq();
	NVIC_SystemReset();
}
