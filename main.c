#include "msp432p401r.h"
#include "msp.h"
#include "tts.h"
#include "ultrasonic.h"
#include "lidar.h"
#include <stdio.h>
#include <string.h>
#include "msp432p401r_classic.h"

// **************************   NOTES    ******************************
//  -- All messages for speak() must end with a '.'
//  -- EMIC functionality messes with ultrasonic sensors --> fixed by changing clock divider for ultrasonics
//  -- Pins layout:
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


void main(void) {

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

 	speak("Hello. How are you?");
 	ultra_init();   // Initialize ultrasonic sensors
 	float distBABY;
 	i2c_init();
	while(1) {
	    ultra_sensors();                        // Get ultrasonic distances
	    checkBoundaries();                      // Check ultrasonic boundaries
	    distBABY = get_distance("in");          // Get lidar distances
	    EUSCI_B0->IE &= ~EUSCI_B_IE_RXIE +           // Disable RX and TX interrupt for I2C
	                ~EUSCI_B_IE_TXIE;
	}
}
