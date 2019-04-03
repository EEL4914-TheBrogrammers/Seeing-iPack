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

void EUSCIB0_IRQHandler() {
    char value = UCB0RXBUF >> 1;

    if (value == '\n') {
        if (strncmp(cmdbuf, "0", 1) == 0) {         // Left trigger
            P5OUT |= BIT4;
            P5OUT &= ~BIT5;
        } else if (strncmp(cmdbuf, "1", 1) == 0){   // Right trigger
            P5OUT |= BIT5;
            P5OUT &= ~BIT4;
        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }

    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
}
