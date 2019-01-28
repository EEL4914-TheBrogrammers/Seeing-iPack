#include "msp432p401r.h"
#include <string.h>

/**
 * main.c
 */

char cmdbuf[20];
int cmd_index=0;

void main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	P1DIR |= BIT0;
	while (P1IN & BIT5);            // If clock sig from mstr stays low, it is not yet in SPI mode

    P1->SEL0 |= BIT5 | BIT6 | BIT7;

	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset
	EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST | // Remain eUSCI state machine in reset
	            EUSCI_B_CTLW0_SYNC |            // Set as synchronous mode
	            EUSCI_B_CTLW0_CKPL |            // Set clock polarity high
	            EUSCI_B_CTLW0_MSB;              // MSB first

	EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__ACLK;// ACLK
	EUSCI_B0->BRW = 0x01;                       // /2,fBitClock = fBRCLK/(UCBRx+1).
	EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Initialize USCI state machine
	EUSCI_B0->IE |= EUSCI_B_IE_RXIE;        // Enable USCI_B0 RX interrupt
	
	SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Remain on LPM after ISR finishes

	// Ensures SLEEPONEXIT takes effect immediately
	__DSB();
	
	// Global interrupt
	__enable_irq();

	// Enable eUSCI_B0 interrupt in NVIC module
	NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);           // Enable interrupts

	__sleep();
	__no_operation();
}

void EUSCIB0_IRQHandler() {
    char value = UCB0RXBUF >> 1;
    if (value == '\n') {
        if (strncmp(cmdbuf, "HELLO WORLD", 11) == 0) {
            P1OUT |= BIT0;
        } else {
            P1OUT &= ~BIT0;
        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }

    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));

    // Echo received data
    EUSCI_B0->TXBUF = 0x0B << 1;        // Test echo 0x0B
}
