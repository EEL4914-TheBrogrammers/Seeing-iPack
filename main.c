#include "msp.h"

/**
 * main.c
 */

#define LEFT        (0)     // Left sensor
#define MIDDLE      (1)     // Middle sensor
#define RIGHT       (2)     // Right sensor

int sensor;                 // Determines which sensor is being used
int prev_sensor;            // Keeps track of previous sensor
int num_of_samples;         // Counts # of samples taken per 100
float sum;                  // Sum of distances to calculate average distance
float dist;
unsigned int up_counter;    // Temp variable to store counter
int bound = 30;

void Timer_initL() {
    TA0CCTL3 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void Timer_initM() {
    TA0CCTL2 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void Timer_initR() {
    TA0CCTL1 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.1)
    TA0CTL |= TASSEL_2 | MC_2 | ID_0;               // SMCLK, continuous mode, clock divider 1
}

void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	P4DIR |= BIT1 + BIT2 + BIT3;    // LED (L, M, R)

	P2DIR |= BIT7 + BIT3 + BIT2;    // Trigger pin (L, M, R)
	P2OUT &= ~BIT7 + ~BIT3 + ~BIT2;

	P2DIR &= ~BIT6 + ~BIT5 + ~BIT4; // Echo pin (L, R)
	P2SEL0 |= BIT6 + BIT5 + BIT4;

	sensor = 0;
	Timer_initL();

    __enable_irq();                     // Enable interrupts
    NVIC_SetPriority(TA0_N_IRQn, 3);    /* set priority to 3 in NVIC */
    NVIC_EnableIRQ(TA0_N_IRQn);         /* enable interrupt in NVIC */

    while(1) {
//        __delay_cycles(50);

        if (sensor == LEFT) {
            P2OUT |= BIT7;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT7;             // Deassert
        } else if (sensor == MIDDLE) {
            P2OUT |= BIT3;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT3;             // Deassert
        } else if (sensor == RIGHT) {
            P2OUT |= BIT2;              // Assert
            __delay_cycles(10);         // 10us wide
            P2OUT &= ~BIT2;             // Deassert
        }
    }
}

void checkBoundaries() {
    if (prev_sensor == LEFT) {
        if(dist <= bound && dist > 0) {
            P4OUT |= BIT1;
            __delay_cycles(10000);
            P4OUT &= ~BIT1;
        }
    }
    if (prev_sensor == MIDDLE) {
        if(dist <= bound && dist > 0) {
            P4OUT |= BIT2;
            __delay_cycles(10000);
            P4OUT &= ~BIT2;
        }
    }
    if (prev_sensor == RIGHT) {
        if(dist <= bound && dist > 0) {
            P4OUT |= BIT3;
            __delay_cycles(10000);
            P4OUT &= ~BIT3;
        }
    }
}

void average() {
    unsigned int distance;

    // Formula: Distance in cm = (Time in uSec)/58
    if (sensor == LEFT) {
        distance = (TA0CCR3 - up_counter)/377;
    } else if (sensor == MIDDLE) {
        distance = (TA0CCR2 - up_counter)/377;
    } else if (sensor == RIGHT) {
        distance = (TA0CCR1 - up_counter)/377;
    }

    // Average the distances for 100 samples to get accurate distance
    if (num_of_samples == 20){
        dist = sum/20.0;
        num_of_samples = 0;
        sum = 0;

        if (sensor == LEFT) {
            TA0CCTL3 &= ~CCIE;  // Disable interrupt left sensor
            sensor = MIDDLE;
            prev_sensor = LEFT;
            Timer_initM();
        } else if (sensor == MIDDLE) {
            TA0CCTL2 &= ~CCIE;  // Disable interrupt right sensor
            sensor = RIGHT;
            prev_sensor = MIDDLE;
            Timer_initR();
        } else if (sensor == RIGHT) {
            TA0CCTL1 &= ~CCIE;  // Disable interrupt right sensor
            sensor = LEFT;
            prev_sensor = RIGHT;
            Timer_initL();
        }
    } else {
        sum += distance;
        num_of_samples ++;
    }
}

void TA0_N_IRQHandler() {
    if (sensor == LEFT) {           // LEFT SENSOR
        if (TA0CCTL3 & CCI) {       // Rising edge
            up_counter = TA0CCR3;   // Copy counter to variable
        } else {                    // Falling edge
            average();
            TA0R = 0;
        }
        TA0CCTL3 &= ~CCIFG;         // Clear interrupt flag - handled
    } else if (sensor == MIDDLE) {  // RIGHT SENSOR
        if (TA0CCTL2 & CCI) {       // Rising edge
            up_counter = TA0CCR2;   // Copy counter to variable
        } else {                    // Falling edge
            average();
            TA0R = 0;
        }
        TA0CCTL2 &= ~CCIFG;         // Clear interrupt flag - handled
    } else if (sensor == RIGHT) {   // RIGHT SENSOR
        if (TA0CCTL1 & CCI) {       // Rising edge
            up_counter = TA0CCR1;   // Copy counter to variable
        } else {                    // Falling edge
            average();
            TA0R = 0;
        }
        TA0CCTL1 &= ~CCIFG;         // Clear interrupt flag - handled
    }
    checkBoundaries();
    __no_operation();

}
