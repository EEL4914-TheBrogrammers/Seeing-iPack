/*
 * spi_msp.c
 */

#include "msp.h"
#include <spi_msp.h>

// SPI initialization
void spi_init() {
    while (P6IN & BIT3);                        // If clock sig from mstr stays low, it is not yet in SPI mode

    P6->SEL0 |= BIT3 | BIT4 | BIT5;             // P6.3 Clk, P1.6 SIMO, P1.7 SOMI

    P5->DIR |= BIT4 | BIT5;

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_SWRST;     // Put eUSCI state machine in reset
    EUSCI_B1->CTLW0 = EUSCI_B_CTLW0_SWRST |     // Remain eUSCI state machine in reset
                EUSCI_B_CTLW0_SYNC |            // Set as synchronous mode
                EUSCI_B_CTLW0_CKPL |            // Set clock polarity high
                EUSCI_B_CTLW0_MSB;              // MSB first

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_SSEL__ACLK;// ACLK
    EUSCI_B1->BRW = 0x01;                       // /2,fBitClock = fBRCLK/(UCBRx+1).
    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Initialize USCI state machine

    cmd_index = 0;
}

// SPI interrupt initialization
void spi_interrupt_init(){
    EUSCI_B1->IE |= EUSCI_B_IE_RXIE;            // Enable USCI_B0 RX interrupt

    __enable_irq();                             // Global interrupt

    // Enable eUSCI_B0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB1_IRQn) & 31); // Enable interrupts
}

void EUSCIB1_IRQHandler() {
    char value = UCB1RXBUF >> 1;

    if (value == '\n') {
        if (strncmp(cmdbuf, "0", 1) == 0) {         // Left trigger
            P5OUT |= BIT4;
            P5OUT &= ~BIT5;
        } else if (strncmp(cmdbuf, "1", 1) == 0) {  // Right trigger
            P5OUT |= BIT5;
            P5OUT &= ~BIT4;
        }
//        else if (strncmp(cmdbuf, "2", 1) == 0) {  // On trigger
//            P5OUT |= BIT6;                          // Place holder, do something
//        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }

    while (!(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG));
}
