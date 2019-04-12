#include <stdio.h>
#include <string.h>

#include "msp.h"
#include "tts.h"
#include "lidar.h"
//#include "spi_msp.h"
//#include "ultrasonic.h"
#include "ultra.h"
//#include "ultra_no_int"
#include "msp432p401r.h"
#include "msp432p401r_classic.h"

// **************************   NOTES    ******************************
//  -- All messages for speak() must end with a '.'
//  -- EMIC functionality messes with sensors --> fixed by changing clock divider for ultrasonics and LiDAR
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
//  -- Delay after LiDAR readings vary directly with distance of readings (increase delay if increasing bounds)

//int arr[5];
//int c = 0;

//void checkBoundaries(void) {
//    if (middle_val <= bound && middle_val > 0) {
//            speak("mid.");
//    }
//    if(left_val <= side_bound*2 && left_val > 0) {
//        speak("left.");
//    }
//    if(right_val <= side_bound*2 && right_val > 0) {
//        speak("right.");
//    }
//}

float middle_val = 0;

void main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer
//    bound = 20;
//    side_bound = 10;
//  go = 0;
//  spi_init();
//  spi_interrupt_init();
//
//  // Wait for on trigger from RPi
//  while (1) {
//      if (go == 1) {
//          break;
//      }
//  }
    speak("Hello. How are you?");
//    ultra_init();   // Initialize ultrasonic sensors
//    i2c_init();
//    manual_reset();
//    __delay_cycles(10000000);
//    speak("Setup complete.");
//    initUltra();
    while(1) {
        initUltra(LEFT);
        float dist = find_distance();
        __no_operation();
        if(dist < 30 && dist > 0) {
            speak("left.");
        }
        dist = 0;
        initUltra(RIGHT);
        dist = find_distance();
        if(dist < 30 && dist > 0) {
            speak("right.");
        }
        dist = 0;
//        ultra_sensors();                        // Get ultrasonic distances
//        ultra_init();
        int i = 0;
        int sumTemp = 0;
        i2c_init();
        for(i = 0; i < 5; i++) {
//            arr[c] = get_distance("in");
//            sumTemp += arr[c];
//            c++;// Get lidar distances
            sumTemp += get_distance("in");
            EUSCI_B0->IE &= ~EUSCI_B_IE_RXIE + ~EUSCI_B_IE_TXIE;       // Disable RX and TX interrupt for I2C
            __delay_cycles(60000);
        }
        middle_val = sumTemp/5;
        if(middle_val < 50 && middle_val > 0) {
            speak("middle.");
        }
//        left_val = 0;
        middle_val = 0;
    }
}
