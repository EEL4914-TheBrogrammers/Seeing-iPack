/*
 * spi_msp.c
 */

#include "msp.h"
#include <spi_msp.h>

// SPI initialization
void spi_init() {
    while (P1IN & BIT5);                        // If clock sig from mstr stays low, it is not yet in SPI mode

    P1->SEL0 |= BIT5 | BIT6 | BIT7;             // P1.5 Clk, P1.6 SIMO, P1.7 SOMI

    P5->DIR |= BIT3 | BIT2;                     // (L, R)
    P5->OUT &= ~BIT3;
    P5->OUT &= ~BIT2;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;     // Put eUSCI state machine in reset
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST |     // Remain eUSCI state machine in reset
                EUSCI_B_CTLW0_SYNC |            // Set as synchronous mode
                EUSCI_B_CTLW0_CKPL |            // Set clock polarity high
                EUSCI_B_CTLW0_MSB;              // MSB first

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SSEL__ACLK;// ACLK
    EUSCI_B0->BRW = 0x01;                       // /2,fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Initialize USCI state machine

    cmd_index = 0;
    start = 0;
    stop = 0;
}

// SPI interrupt initialization
void spi_interrupt_init(){
    EUSCI_B0->IE |= EUSCI_B_IE_RXIE;            // Enable USCI_B0 RX interrupt

    __enable_irq();                             // Global interrupt

    // Enable eUSCI_B0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31); // Enable interrupts
}

void EUSCIB0_IRQHandler() {
    char value = UCB0RXBUF >> 1;

    if (value == '\n') {
        if (strncmp(cmdbuf, "0", 1) == 0) {         // Left trigger
            P5OUT |= BIT3;
            P5OUT &= ~BIT2;
        } else if (strncmp(cmdbuf, "1", 1) == 0) {  // Right trigger
            P5OUT |= BIT2;
            P5OUT &= ~BIT3;
        } else if (strncmp(cmdbuf, "2", 1) == 0) {  // Stop buzzers
            P5OUT &= ~BIT3;
            P5OUT &= ~BIT2;
            stop = 1;
            start = 0;
        } else if (strncmp(cmdbuf, "3", 1) == 0) {  // Start program
            P5OUT &= ~BIT3;
            P5OUT &= ~BIT2;
            start = 1;
            stop = 0;
        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }

    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG));
}
