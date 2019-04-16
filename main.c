#include <stdio.h>
#include <string.h>
#include <spi_msp.h>
#include "msp.h"
#include "main.h"
#include "tts.h"
#include "lidar.h"
#include "spi_msp.h"
#include "ultra.h"
#include "msp432p401r.h"
#include "msp432p401r_classic.h"

/**************************************************
 *                                                *
 *                    NOTES                       *
 *                                                *
 **************************************************/
//  -- All messages for speak() must end with a '.'
//  -- EMIC functionality messes with sensors --> fixed by changing clock divider for ultrasonics and LiDAR
//  -- Delay after LiDAR readings vary directly with distance of readings (increase delay if increasing bounds)
//  -- Pins layout:
//      - Text-to-Speech (UART)
//          SOUT = P1.2 (RXD)
//          SIN = P1.3 (TXD)
//      - Raspberry Pi (SPI)
//          clk = P1.5
//          SIMO = P1.6
//          SOMI = P1.7
//      - Left ultrasonic sensor (Timer A 0.3)
//          echo = P2.4
//          trig = P2.2
//      - Middle ultrasonic sensor (Timer A 0.2)
//          echo = P2.5
//          trig = P2.3
//      - Right ultrasonic sensor (Timer A 0.1)
//          echo = P2.6
//          trig = P2.7
//      - LiDAR sensor (USCI B1 I2C)
//          SDA = P6.4
//          SCL = P6.5


/**************************************************
 *                                                *
 *              GLOBAL VARIABLES                  *
 *                                                *
 **************************************************/


/**************************************************
 *                                                *
 *                 FUNCTIONS                      *
 *                                                *
 **************************************************/
//void check_bounds(float distance, char* string) {
//    if(distance < bounds && distance > 0) {
//        speak(string);
//    }
//}

void main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // Welcome messages
//    speak("Yaaaaas queen. Let's get that shmoney.");
    speak("Hello! Welcome to Seeing eye pack.");
    speak("Now configuring....");

    // SPI configuration for RPi
    spi_init();
    spi_interrupt_init();

    while(1) {
        // Wait on RPi
        while(start == 0);

        // Get distance measurement from left sensor
        initUltra(LEFT);
        dist = find_distance();
        __no_operation();
//        check_bounds(dist, "left.");
        if(dist < 30 && dist > 0 && stop == 0 && cam_config == 0) {
            speak("left.");
        }
        dist = 0;

        // Get distance measurement from right sensor
        initUltra(RIGHT);
        dist = find_distance();
//        check_bounds(dist, "right.");
        if(dist < 30 && dist > 0 && stop == 0 && cam_config == 0) {
            speak("right.");
        }
        dist = 0;

        int i = 0;
        int sumTemp = 0;
        i2c_init();
        for(i = 0; i < 5; i++) {
            sumTemp += get_distance("in");
            EUSCI_B0->IE &= ~EUSCI_B_IE_RXIE + ~EUSCI_B_IE_TXIE;       // Disable RX and TX interrupt for I2C
            __delay_cycles(120000);
        }
        dist = sumTemp/5;
//        check_bounds(dist, "middle.");
        if(dist < 50 && dist > 0 && stop == 0 && cam_config == 0) {
            speak("middle.");
        }
        dist = 0;
    }
}
