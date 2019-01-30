#include "msp432p401r.h"
#include "msp.h"
#include <string.h>
#include "msp432p401r_classic.h"

unsigned int up_counter;

void Timer_init() {
    TA0CCTL1 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;    // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;   // SMCLK, continuous mode, clock divider 1
}

void main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	P2DIR |= BIT6;  // Trigger pin
	P2OUT &= ~BIT6; // Keep trigger low

    P2DIR &= ~BIT4;  // Timer pin as input
    P2SEL0 |= BIT4;  // Timer pin (TA)

    Timer_init();

    __enable_irq(); // Enable interrupts
    NVIC_SetPriority(TA0_N_IRQn, 3);    /* set priority to 3 in NVIC */
    NVIC_EnableIRQ(TA0_N_IRQn);         /* enable interrupt in NVIC */

    while(1) {
        P2OUT |= BIT6;              // assert
        __delay_cycles(10);         // 10us wide
        P2OUT &= ~BIT6;              // deassert
        __delay_cycles(70000);      // 60ms measurement cycle
    }
}

void TA0_N_IRQHandler() {
    unsigned int distance_cm;
    if (TA0CCTL1 & CCI) {  // Rising edge
            up_counter = TA0R;      // Copy counter to variable
    } else {    // Falling edge
            // Formula: Distance in cm = (Time in uSec)/58
            distance_cm = (TA0R - up_counter)/377;
            __no_operation();
    }
    TA0CCTL1 &= ~CCIFG;           // Clear interrupt flag - handled
}
