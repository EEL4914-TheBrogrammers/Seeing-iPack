#include "msp.h"
#include <stdio.h>
#include <string.h>
#include "tts.h"

// *******************************   NOTES    ***********************************
//  -- Baud rate calculations
//        12000000/(16*9600) = 78.125
//        Fractional portion = 0.125
//        User's Guide Table 21-4: UCBRSx = 0x10
//        UCBRFx = int ( (78.125-78)*16) = 2
//  -- All message transmits must begin with a CR or LF (\n);
//  -- All message transmits must terminate with a CR  or LF (\n)


// **************************   Global Variables    ******************************
char* message;
int size = 0;
int wait = 0;


// ******************************   Functions    *********************************
void init(void) {
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_B_CTLW0_SSEL__SMCLK;      // Configure eUSCI clock source for SMCLK
    EUSCI_A0->BRW = 78;                     // 12000000/16/9600
    EUSCI_A0->MCTLW = (2 << EUSCI_A_MCTLW_BRF_OFS) |
            EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;    // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;        // Enable USCI_A0 RX interrupt
}

void clockSetup(void) {
    CS->KEY = CS_KEY_VAL;                   // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3;           // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                            // Lock CS module from unintended accesses
}

void interruptsConfig(void) {
    __enable_irq();                                 // Enable global interrupt
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);     // Enable eUSCIA0 interrupt in NVIC module
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;            // Enable sleep on exit from ISR
    __DSB();                                        // Ensures SLEEPONEXIT occurs immediately
}

void speak(char* string) {
    // Global variable initializations
    wait = 0;
    size = strlen(string);
    message = string;

    // Configure clock
    clockSetup();

    // Configure UART pins
    P1->SEL0 |= BIT2 | BIT3;                // Set P1.2 (RXD), set 1.3 (TXD)

    // Configure UART
    init();

    // Configure interrupts
    interruptsConfig();

    // Start speech
    while(UCA0STATW & UCBUSY);
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
    EUSCI_A0->TXBUF = '\n';

    // Enter LPM0
    __sleep();
 }

// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG) {
        // Check if the TX buffer is empty first
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

        // Echo the received character back
        char character = EUSCI_A0->RXBUF;
        if (character == ':') {
            if(wait == 1) {
                EUSCI_A0->IE &= ~EUSCI_A_IE_RXIE;
                SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;
            } else {
                while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                EUSCI_A0->TXBUF = 'S';
                int i = 0;
                for(i = 0; i < size; i++) {
                    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
                    EUSCI_A0->TXBUF = message[i];
                }
                EUSCI_A0->TXBUF = '\n';
                wait++;
            }
        }
        EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;
    }
}
