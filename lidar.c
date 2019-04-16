/*
 * lidar.c
 */

#include "msp.h"
#include <lidar.h>
#include <stdint.h>
#include <string.h>

// Manually toggle SCL pin to allow slave to release SDA
void manual_reset() {
    volatile uint32_t i;

    // Change P1.7 from SCL to GPIO pin
    P6->SEL0 &= ~BIT5;
    P6->DIR |= BIT5;

    for(i = 0; i < 9; i++) {
        __delay_cycles(1);
        P6->OUT |= BIT5;

        __delay_cycles(1);
        P6->OUT &= ~BIT5;
    }

    // Change P1.7 from GPIO to SCL pin
    P6->OUT |= BIT5;
    P6->DIR &= ~BIT5;
    P6->SEL0 |= BIT5;
}

// Delay function in ms for 30 MHz clock
void delay_ms(unsigned int delay) {
    while (delay--) {
        __delay_cycles(3000);
    }
}

// I2C transmit function
void i2c_tx(short address, short data) {
    volatile uint32_t i;

    // Set slave address register and data address
    tx_addr = address;
    tx_data = data;

    counter = 1;

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR + EUSCI_B_CTLW0_TXSTT;  // Transmit start condition

    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    while (!(EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP));           // Ensure stop condition got sent
    while (EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0);

    for (i = 400; i > 0; i--);
}

// I2C receive function
unsigned short i2c_rx(short address) {
    volatile uint32_t i;
    tx_addr = address;

    counter = 4;

    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TR + EUSCI_B_CTLW0_TXSTT;  // Transmit start condition

    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    while (!(EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTP));           // Ensure stop condition got sent
    while (EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0);

    for (i = 400; i > 0; i--);

//    if(address == STATUS) {
//        counter = 7;
//    } else {
        counter = 6;
//    }

    EUSCI_B1->CTLW0 &= ~EUSCI_B_CTLW0_TR;           //Set Receiver bit
    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTT;         //Set start bit

    while ((EUSCI_B1->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    while (EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0);

    counter = 0;

    for (i = 400; i > 0; i--);

    return lvalue;
}

// I2C initialization function
void i2c_init() {
    P6->SEL0 |= BIT4 | BIT5;                    // I2C pins (P1.6 SDA, P1.7 SCL)

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB1_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B1->CTLW0 |= EUSCI_A_CTLW0_SWRST;     // Software reset enabled
    EUSCI_B1->CTLW0 = EUSCI_A_CTLW0_SWRST |     // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |              // I2C mode
            EUSCI_B_CTLW0_MST |                 // Master mode
            EUSCI_B_CTLW0_SYNC |                // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;          // SMCLK

    EUSCI_B1->BRW = 120;                         // Baud rate = SMCLK / 30 = 100kHz
    EUSCI_B1->I2CSA = SLAVE_ADDR;               // Slave address
    EUSCI_B1->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Release eUSCI from reset

    EUSCI_B1->IE |= EUSCI_B_IE_RXIE |           // Enable RX and TX interrupt
            EUSCI_B_IE_TXIE;

    avg_count = 0;
    bias_counter = 0;
}

// Function to get distance reading from LiDAR
float get_distance(char* type) {
    float average_cm;
    float average_in;

    while (avg_count <= SAMPLES) {

        if(bias_counter == 0) {
            i2c_tx(ACQ_COMMAND, START_MEASURE);     // (TX) Take distance measurement w/ receiver bias
        } else {
            i2c_tx(ACQ_COMMAND, NO_BIAS);     // (TX) Take distance measurement w/o receiver bias
        }

        bias_counter++;
        bias_counter = bias_counter % 100;

        i2c_rx(STATUS);
        while(lvalue & 0x01) {
            i2c_rx(STATUS);
        }

        i2c_rx(HIGH);

        if(lvalue > 0) {
            lvalue = 255;
        } else {
            i2c_rx(LOW);
        }


//        i2c_rx(DISTANCE);                       // (TX + RX) Receive distance measurement



//        i2c_tx(ACQ_COMMAND, RESET);             // (TX) Reset all registers to default values

        // Average the distances for accurate distance
        if (avg_count == SAMPLES){
            average_cm = sum/SAMPLES;           // Average sum of distances
            avg_count = 0;                      // Reset average counter
            sum = 0;                            // Reset sum

            if (!strcmp(type, "cm")) {          // Return average in cm
                return average_cm;
            } else if (!strcmp(type, "in")) {   // Return average in in
                average_in = average_cm/2.54;
                return average_in;
            } else {                            // Return error
                return -1;
            }
        } else {
            sum += lvalue;                      // Sum distance values
            avg_count++;
        }
    }
}

// I2C interrupt service routine
void EUSCIB1_IRQHandler(void)
{
    if (!(EUSCI_B1->IFG & EUSCI_B_IFG_NACKIFG)) {
        switch(counter){
            case 1:{    // Transmit register address
                EUSCI_B1->TXBUF = tx_addr;
                counter++;
                break;
            }
            case 2:{    // Transmit data
                EUSCI_B1->TXBUF = tx_data;
                counter++;
                break;
            }
            case 3:{    // Transmit stop bit and clear TXIFG
                EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
                break;
            }
            case 4:{    // Transmit register address to receive
                if(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0){
                    EUSCI_B1->TXBUF = tx_addr;
                    counter++;
                }
                break;
            }
            case 5:{    // Transmit stop bit and clear TXIFG to receive
                if(EUSCI_B1->IFG & EUSCI_B_IFG_TXIFG0){
                    EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                    EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
                }
                break;
            }
            case 6:{    // Receive distance measurement high byte
                if(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0){
                    hvalue = EUSCI_B1->RXBUF;
                }
                counter++;
                break;
            }
            default:{   // Receive distance measurement low byte, transmit stop bit, and clear RX and TX flags
                if(EUSCI_B1->IFG & EUSCI_B_IFG_RXIFG0){
                    lvalue = EUSCI_B1->RXBUF;
                }

                EUSCI_B1->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                EUSCI_B1->IFG &= ~EUSCI_B_IFG_TXIFG;
                EUSCI_B1->IFG &= ~EUSCI_B_IFG_RXIFG;
                break;
            }
        }
    }
}
