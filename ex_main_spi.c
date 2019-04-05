#include "msp432p401r.h"
#include <spi_msp.h>

/**
 * main.c
 */

void main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	spi_init();             // Initialize SPI
	spi_interrupt_init();   // Initialize SPI interrupt

	while(1) {
	    int i = 0;
	}
}
