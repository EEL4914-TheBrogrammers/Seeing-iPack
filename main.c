/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP432P401 Demo  - eUSCI_B0 I2C Master RX multiple bytes from MSP432 Slave
//
//  Description: This demo connects two MSP432's via the I2C bus. The master
//  reads 5 bytes from the slave. This is the MASTER CODE. The data from the slave
//  transmitter begins at 0 and increments with each transfer.
//  The USCI_B0 RX interrupt is used to know when new data has been received.
//
//    *****used with "msp432p401x_euscib0_i2c_11.c"****
//
//                                 |    |
//               MSP432P401      10k  10k     MSP432P401
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//            |     P1.6/UCB0SDA|<-|----|->|P1.6/UCB0SDA     |
//            |                 |  |       |                 |
//            |                 |  |       |                 |
//            |     P1.7/UCB0SCL|<-|------>|P1.7/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//
//   William Goh
//   Texas Instruments Inc.
//   June 2016 (updated) | June 2014 (created)
//   Built with CCSv6.1, IAR, Keil, GCC
//******************************************************************************
#include "msp.h"
#include <stdint.h>

short tx_addr;      // Address to transmit
short tx_data;      // Data to transmit
short lvalue;       // Low byte distance measurement
short hvalue;       // High byte distance measurement

int counter;        // Keeps track of what to do in interrupt
int avg_count;      // Counter to calculate average distance
float sum;          // Sum of 100 distances to calculate average distance

void manual_reset() {
    volatile uint32_t i;
    P1->SEL0 &= ~BIT7;      // Change P1.7 to GPIO pin
    P1->DIR |= BIT7;

    for(i = 0; i < 9; i++) {
        volatile uint32_t j;
        volatile uint32_t k;

        // Delay between transmissions
        __delay_cycles(1);
        P1->OUT |= BIT7;

        // Delay between transmissions
        __delay_cycles(1);
        P1->OUT &= ~BIT7;
    }

    P1->OUT |= BIT7;
    P1->DIR &= ~BIT7;

    P1->SEL0 |= BIT7;       // Change P1.7 back to SCL pin
}

void i2c_tx(short address, short data){
    // Set slave address register and data address
    tx_addr = address;
    tx_data = data;

    counter = 1;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR + EUSCI_B_CTLW0_TXSTT;  // Transmit start condition
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    while (!(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP));              // Ensure stop condition got sent

    while (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0);
}

unsigned short i2c_rx(short address){
    volatile uint32_t i;
    tx_addr = address;

    counter = 4;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR + EUSCI_B_CTLW0_TXSTT;  // Transmit start condition

    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));
    while (!(EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP));              // Ensure stop condition got sent
    while (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0);


    // Arbitrary delay before transmitting the next byte
    for (i = 250; i > 0; i--);

    counter = 6;
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;           //Set Receiver bit
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;         //Set start bit
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    while (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0);

    counter = 0;

    return lvalue;
}

void delay_ms(unsigned int delay){
    while (delay--) {
        __delay_cycles(3000);
    }
}

int main(void)
{
    volatile uint32_t i;

    WDT_A->CTL = WDT_A_CTL_PW |             // Stop watchdog timer
            WDT_A_CTL_HOLD;

    manual_reset();


    delay_ms(44);


    P1->SEL0 |= BIT6 | BIT7;                // I2C pins

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // Master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK

    EUSCI_B0->BRW = 30;                     // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0->I2CSA = 0x0062;               // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Release eUSCI from reset

    EUSCI_B0->IE |= EUSCI_B_IE_RXIE |       // Enable receive interrupt
            EUSCI_B_IE_TXIE;

    avg_count = 0;

    while (1)
    {
        i2c_tx(0x00, 0x04);

        // Arbitrary delay before transmitting the next byte
        for (i = 250; i > 0; i--);

        i2c_rx(0x8F);

        // Arbitrary delay before transmitting the next byte
        for (i = 250; i > 0; i--);

        i2c_tx(0x00, 0x00);

        // Arbitrary delay before transmitting the next byte
        for (i = 250; i > 0; i--);

        // Average the distances for 100 samples to get accurate distance
        if (avg_count == 1000){
            float average_cm = sum/1000.0;
            float average_in = average_cm/2.54;
            avg_count = 0;
            sum = 0;
        } else {
            // Discard bad distance readings
            sum += lvalue;
            avg_count++;
        }
    }
}

// I2C interrupt service routine
void EUSCIB0_IRQHandler(void)
{
    if (!(EUSCI_B0->IFG & EUSCI_B_IFG_NACKIFG)) {
        switch(counter){
            case 1:{
                EUSCI_B0->TXBUF = tx_addr;
                counter++;
                break;
            }
            case 2:{
                EUSCI_B0->TXBUF = tx_data;
                counter++;
                break;
            }
            case 3:{
                EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG;
                break;
            }
            case 4:{
                if(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0){
                    EUSCI_B0->TXBUF = tx_addr;
                    counter++;
                }
                break;
            }
            case 5:{
                if(EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0){
                    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                    EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG;
                }
                break;
            }
            case 6:{
                if(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0){
                    hvalue = EUSCI_B0->RXBUF;
                }
                counter++;
                break;
            }
            default:{
                if(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0){
                    lvalue = EUSCI_B0->RXBUF;
                }

                EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
                EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG;
                EUSCI_B0->IFG &= ~EUSCI_B_IFG_RXIFG;
                break;
            }
        }
    }
}
