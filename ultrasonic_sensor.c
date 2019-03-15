#include "msp432p401r.h"
#include "msp.h"
#include <string.h>
#include "msp432p401r_classic.h"

/**************************************************
 *                                                *
 *               Global Variables                 *
 *                                                *
 **************************************************/

int bound = 70;
int sensor = 1;
int countR, countM, countL;                  // Counts # of samples taken per 100
float sumR, sumM, sumL;                // Sum of distances to calculate average distance
float distR, distM, distL;
int flip = 0;
unsigned int up_counterR, up_counterM, up_counterL;    // Temp variable to store counter

void Timer_initL() {
    TA0CCTL3 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void Timer_initM() {
    TA0CCTL2 |= CM_3 | SCS | CCIS_0 | CAP;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void Timer_initR() {
    TA0CCTL1 |= CM_3 | SCS | CCIS_0 | CAP;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.1)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop watchdog timer

    distL = 0;
    distR = 0;
    distM = 0;
    countL = 0;
    countR = 0;
    countM = 0;
    sumL = 0;
    sumR = 0;
    sumM = 0;

    P4DIR |= BIT1 + BIT2 + BIT3; // LED pins (L, M, R)

    P2DIR |= BIT7 + BIT3 + BIT2;  // Trigger pin (L, M, R)
    P2OUT &= ~BIT7 + ~BIT3 + ~BIT2; // Keep trigger low (M, R)

    P2DIR &= ~BIT6 + ~BIT5 + ~BIT4;  // Timer pin as input (L ,M, R)
    P2SEL0 |= BIT6 + BIT5 + BIT4;  // Timer pin (TA) (L, M, R)

    Timer_initL();

    __enable_irq(); // Enable interrupts
    NVIC_SetPriority(TA0_N_IRQn, 3);    /* set priority to 3 in NVIC */
    NVIC_EnableIRQ(TA0_N_IRQn);         /* enable interrupt in NVIC */

    while(1) {
        if(sensor == 1) {   // Left sensor
            P2OUT |= BIT7;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT7;             // Deassert
        } else if(sensor == 2) {   // Middle sensor
            P2OUT |= BIT3;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT3;             // Deassert
        } else if (sensor == 3) {
            P2OUT |= BIT2;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT2;             // Deassert
        }
//        __delay_cycles(6000);
//        __delay_cycles(9000);      // 60ms measurement cycle
    }
}

void checkBoundaries() {
    if(distL <= bound && distL > 0) {
        P4OUT |= BIT1;
    } else {
        P4OUT &= ~BIT1;
    }
    if(distM <= bound && distM > 0) {
        P4OUT |= BIT2;
    } else {
        P4OUT &= ~BIT2;
    }
    if(distR <= bound && distR > 0) {
        P4OUT |= BIT3;
    } else {
        P4OUT &= ~BIT3;
    }
}

void averageL() {
    // Formula: Distance in cm = (Time in uSec)/58
    unsigned int distanceL;
    distanceL = (TA0CCR3 - up_counterL)/377;

    // Average the distances for 100 samples to get accurate distance
    if (countL == 50){
        distL = sumL/50.0;
        countL = 0;
        sumL = 0;
        TA0CCTL3 &= ~CCIE;  // Disable interrupt for left sensor
        sensor++;           // Increment sensor count
        flip = 0;
        Timer_initM();      // Init middle sensor
        TA0CCTL2 |= CCIE;   // Enable interrupt for middle sensor
    } else {
        sumL += distanceL;
        countL ++;
    }
}

void averageM() {
    // Formula: Distance in cm = (Time in uSec)/58
    unsigned int distanceM;
    distanceM = (TA0CCR2 - up_counterM)/377;

    // Average the distances for 100 samples to get accurate distance
    if (countM == 50){
        distM = sumM/50.0;
        countM = 0;
        sumM = 0;
        TA0CCTL2 &= ~CCIE;  // Disable interrupt for middle sensor
        if(flip == 0) {
            sensor++;           // Increment sensor count
            Timer_initR();      // Init right sensor
            TA0CCTL1 |= CCIE;   // Enable interrupt for right sensor
        } else {
            sensor--;
            Timer_initL();
        }
    } else {
        sumM += distanceM;
        countM ++;
    }
}

void averageR() {
    // Formula: Distance in cm = (Time in uSec)/58
    unsigned int distanceR;
    distanceR = (TA0CCR1 - up_counterR)/377;

    // Average the distances for 100 samples to get accurate distance
    if (countR == 50){
        distR = sumR/50.0;
        countR = 0;
        sumR = 0;
        TA0CCTL1 &= ~CCIE;
        sensor--;
        flip = 1;
        Timer_initM();      // Init middle sensor
        TA0CCTL2 |= CCIE;   // Enable interrupt for middle sensor
    } else {
        sumR += distanceR;
        countR ++;
    }
}

void TA0_N_IRQHandler() {
    if(sensor == 1) { // Left sensor
        if (TA0CCTL3 & CCI) {       // Rising edge
            up_counterL =  TA0CCR3;  // Copy counter to variable
        } else {                    // Falling edge
            averageL();
            TA0R = 0;
        }
        TA0CCTL3 &= ~CCIFG;           // Clear interrupt flag - handled
    } else if(sensor == 2) {   // Middle sensor
        if (TA0CCTL2 & CCI) {       // Rising edge
            up_counterM = TA0CCR2;  // Copy counter to variable
        } else {                    // Falling edge
            averageM();
            TA0R = 0;
        }
        TA0CCTL2 &= ~CCIFG;           // Clear interrupt flag - handled
    } else if(sensor == 3) {    // Right sensor
        if (TA0CCTL1 & CCI) {       // Rising edge
            up_counterR = TA0CCR1;  // Copy counter to variable
        } else {                    // Falling edge
            averageR();
            TA0R = 0;
        }
        TA0CCTL1 &= ~CCIFG;           // Clear interrupt flag - handled
    }
    checkBoundaries();
    __no_operation();
}

//R1 = 1700 connected to echo
//R2 = 3300 connected to grounds
