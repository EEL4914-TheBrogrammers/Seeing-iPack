/*
 * ultra.c
 *
 *  Created on: Apr 11, 2019
 *      Author: michelle
 */
#include "msp.h"
#include "ultra.h"
#include <stdio.h>
#include <string.h>

void init_timer_left(void) {
    TA0CCTL3 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.3)
    TA0CTL |= TASSEL_2 | MC_2 | ID_2 | TACLR;               // SMCLK, continuous mode, clock divider 4
}

void init_timer_right() {
    TA0CCTL1 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.3)
    TA0CTL |= TASSEL_2 | MC_2 | ID_2 | TACLR;               // SMCLK, continuous mode, clock divider 4
}

void initUltra(int sensor) {
    // Set global variables
    side = sensor;
    distance = 0;
    int_count = 0;
    rising = 0;
    falling = 0;

    // Initialize timer modules
    if(side == LEFT) {
        P2DIR |= BIT7;      // Trigger pin (L, R)
        P2OUT &= ~BIT7;
        P2DIR &= ~BIT6;     // Echo pin (L, R)
        P2SEL0 |= BIT6;
        init_timer_left();
    } else if (side == RIGHT) {
        P2DIR |= BIT2;      // Trigger pin (L, R)
        P2OUT &= ~BIT2;
        P2DIR &= ~BIT4;     // Echo pin (L, R)
        P2SEL0 |= BIT4;
        init_timer_right();
    }

    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);
}

int counting = 0;

float find_distance(void) {
    int_count = 0;

    // Send trigger signal
    if(side == LEFT) {
        P2OUT |= BIT7;              // Assert echo
        __delay_cycles(100);        // 10us wide
        P2OUT &= ~BIT7;
    } else if (side == RIGHT) {
        P2OUT |= BIT2;              // Assert echo
        __delay_cycles(100);        // 10us wide
        P2OUT &= ~BIT2;
    }

    // Wait for echo signal
    __delay_cycles(1200000);

    if(side == LEFT) {
        TA0CCTL3 &= ~CCIE;
        P2SEL0 &= ~BIT6;
    } else if (side == RIGHT) {
        TA0CCTL1 &= ~CCIE;
        P2SEL0 &= ~BIT4;
    }

    return distance;
}

void TA0_N_IRQHandler(void) {
    if(side == LEFT) {
        if(rising != TA0CCR3) {
            if(int_count == 0) {        // Rising edge
                rising = TA0CCR3;
                int_count++;
            } else {                    // Falling edge
                falling = TA0CCR3;
                float temp = falling;
                if(falling < rising) {
                    temp += 65535;
                }
                distance = (temp - rising)/444;
                TA0CCTL3 &= ~CCIE;
            }
        }
        TA0CCTL3 &= ~CCIFG;
    } else if (side == RIGHT) {
        if(rising != TA0CCR1) {
            if(int_count == 0) {        // Rising edge
                rising = TA0CCR1;
                int_count++;
            } else {                    // Falling edge
                falling = TA0CCR1;
                float temp = falling;
                if(falling < rising) {
                    temp += 65535;
                }
                distance = (temp - rising)/444;
                TA0CCTL1 &= ~CCIE;
            }
        }
        TA0CCTL1 &= ~CCIFG;
    }
}
