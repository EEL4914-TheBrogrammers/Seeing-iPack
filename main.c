#include "msp.h"
#include <stdio.h>
#include <string.h>

/**
 * main.c
 */

char test[2] = {'N', '2'};

char message[2] = {'H', 'i'};

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses

    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // set P1.2 (RXD), set 1.3 (TXD)

    // Configure UART
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
//            EUSCI_B_CTLW0_MSB |
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    // Baud Rate calculation
    // 12000000/(16*9600) = 78.125
    // Fractional portion = 0.125
    // User's Guide Table 21-4: UCBRSx = 0x10
    // UCBRFx = int ( (78.125-78)*16) = 2
    EUSCI_A0->BRW = 78;                     // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIA0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);

    // Enable sleep on exit from ISR
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

    // Ensures SLEEPONEXIT occurs immediately
    __DSB();

    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 'S';
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 'h';
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 'e';
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = 'e';

    // Enter LPM0
    __sleep();
    __no_operation();                       // For debugger
}

// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

        // Echo the received character back
//        char rx_char = EUSCI_A0->RXBUF;
        char character = EUSCI_A0->RXBUF;
//        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
        if (character == ':') {
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 'S';
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 'h';
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 'e';
            while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
            EUSCI_A0->TXBUF = 'e';
        }
    }
}
