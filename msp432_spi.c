#include "msp432p401r.h"
#include "msp.h"
//#include <ti/drivers/GPIO.h>
//#include <ti/drivers/SPI.h>
#include <string.h>
#include "msp432p401r_classic.h"

/**
 * main.c
 */

char cmdbuf[20];
char cmd_index=0;

void main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	
	P1DIR |= BIT0;
	while (P1IN & BIT5);                   // If clock sig from mstr stays low,
	                                            // it is not yet in SPI mode
	flash_spi_detected();                 // Blink 3 times
	
	P1SEL0 = BIT5 + BIT6 + BIT7;     // clk + SIMO + SOMI
	P1SEL1 = BIT5 + BIT6 + BIT7;    // clk + SIMO + SOMI
	
	UCB0CTLW0 = UCSWRST;            // Enable software reset
	UCB0CTLW0 |= UCMSB + UCSYNC;    // 3-pin, 8-bit, most sig bit, synchronous mode
	UCB0CTLW0 &= ~UCSWRST;
	
	UCB0IE |= UCRXIE;                // Enable RX interrupt
	
	__enable_interrupt();           // Enable interrupts
}

//__attribute__((interrupt(USCIAB0RX_VECTOR))) void USCI0RX_ISR (void) {
//    
//}

#pragma vector = eUSCI_B0_VECTOR
__interrupt void eUSCI_B0_ISR() {
    char value = UCB0RXBUF;
    if (value == '\n') {
        if (strncmp(cmdbuf, "HELLO WORLD", 11) == 0) {
            P1OUT |= BIT0;
        } else {
            P1OUT &= ~BIT0;
        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }
}
