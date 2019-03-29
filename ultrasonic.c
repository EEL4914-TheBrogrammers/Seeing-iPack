#include "msp.h"
#include "ultrasonic.h"
#include <stdbool.h>

// *******************************   NOTES    ***********************************


// ******************************   Functions    *********************************
void Timer_initL(void) {
    TA0CCTL3 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_2;               // SMCLK, continuous mode, clock divider 4
}

void Timer_initM(void) {
    TA0CCTL2 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.2)
    TA0CTL |= TASSEL_2 | MC_2 | ID_2;               // SMCLK, continuous mode, clock divider 4
}

void Timer_initR(void) {
    TA0CCTL1 |= CM_3 | SCS | CCIS_0 | CAP | CCIE;   // Capture on both edges, synchronous capture, CCI0A input signal, enable interrupt (A0.1)
    TA0CTL |= TASSEL_2 | MC_2 | ID_2;               // SMCLK, continuous mode, clock divider 4
}

void ultra_init(void) {
    P2DIR |= BIT7 + BIT3 + BIT2;    // Trigger pin (L, M, R)
    P2OUT &= ~BIT7 + ~BIT3 + ~BIT2;

    P2DIR &= ~BIT6 + ~BIT5 + ~BIT4; // Echo pin (L, R)
    P2SEL0 |= BIT6 + BIT5 + BIT4;

    bound = 7;
    samples = 5;
    speakL = false;
    speakM = false;
    speakR = false;

    sensor = 0;
    Timer_initL();

    NVIC_SetPriority(TA0_N_IRQn, 3);    /* set priority to 3 in NVIC */
    NVIC_EnableIRQ(TA0_N_IRQn);         /* enable interrupt in NVIC */
}

void ultra_sensors(void) {
    Timer_initL();
    if (sensor == LEFT) {
        P2OUT |= BIT7;              // Assert echo
        __delay_cycles(10);         // 10us wide
        P2OUT &= ~BIT7;             // Deassert
    } else if (sensor == MIDDLE) {
        P2OUT |= BIT3;              // Assert echo
        __delay_cycles(10);         // 10us wide
        P2OUT &= ~BIT3;             // Deassert
    } else if (sensor == RIGHT) {
        P2OUT |= BIT2;              // Assert echo
        __delay_cycles(10);         // 10us wide
        P2OUT &= ~BIT2;             // Deassert
    }
}

void checkBoundaries(void) {
    if (prev_sensor == LEFT) {              // Left sensor check
        if(dist <= bound && dist > 0) {     // Motor vibration
            if(!speakL) {
                speak("left.");
                speakL = true;
            }
        } else {
            speakL = false;
        }
    }
    if (prev_sensor == MIDDLE) {            // Middle sensor check
        if(dist <= bound && dist > 0) {     // Motor vibration
            if(!speakM) {
                speak("middle.");
                speakM = true;
             }
        } else {
            speakM = false;
        }
    }
    if (prev_sensor == RIGHT) {             // Right sensor check
        if(dist <= bound && dist > 0) {     // Motor vibration
            if(!speakR) {
                speak("right.");
                speakR = true;
            }
        } else {
            speakR = false;
        }
    }
}

void average(void) {
    unsigned int distance;

    // Formula: Distance in cm = (Time in uSec)/58
    if (sensor == LEFT) {
        distance = (TA0CCR3 - up_counter)/377;
    } else if (sensor == MIDDLE) {
        distance = (TA0CCR2 - up_counter)/377;
    } else if (sensor == RIGHT) {
        distance = (TA0CCR1 - up_counter)/377;
    }

    // Average the distances for 5 samples to get accurate distance
    if (num_of_samples == samples){
        dist = sum/samples;
        num_of_samples = 0;
        sum = 0;

        if (sensor == LEFT) {
            TA0CCTL3 &= ~CCIE;  // Disable interrupt left sensor
            sensor = MIDDLE;
            prev_sensor = LEFT;
            Timer_initM();      // Initialize timer for middle sensor
        } else if (sensor == MIDDLE) {
            TA0CCTL2 &= ~CCIE;  // Disable interrupt right sensor
            sensor = RIGHT;
            prev_sensor = MIDDLE;
            Timer_initR();      // Initialize timer for right sensor
        } else if (sensor == RIGHT) {
            TA0CCTL1 &= ~CCIE;  // Disable interrupt right sensor
            sensor = LEFT;
            prev_sensor = RIGHT;
        }
    } else {
        sum += distance;
        num_of_samples ++;
    }
}

void TA0_N_IRQHandler(void) {
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
    __no_operation();

}
