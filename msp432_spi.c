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

/** Delay function. **/
void delay(unsigned int d) {
  int i;
  for (i = 0; i<d; i++) {
    __no_operation();
  }
}

void flash_spi_detected(void) {
    int i=0;
    P1OUT = 0;
    for (i=0; i < 6; ++i) {
        P1OUT = ~P1OUT;
        delay(0x4fff);
        delay(0x4fff);
    }
}

void main(void) {
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	
	P1DIR |= BIT0;
	while (P1IN & BIT6);                   // If clock sig from mstr stays low, it is not yet in SPI mode
	flash_spi_detected();                 // Blink 3 times
	
	P2SEL0 = BIT3 + BIT2;       // SIMO + SOMI
	P2SEL1 = BIT3 + BIT2;       // SIMO + SOMI

	P1SEL0 = BIT6;              // SDA
	P1SEL1 = BIT6;              // SDA
	
	UCA1CTLW0 = UCSWRST;            // Enable software reset
	UCA1CTLW0 |= UCMSB + UCSYNC;    // 3-pin, 8-bit, most sig bit, synchronous mode
	UCA1CTLW0 &= ~UCSWRST;
	
	UCA1IE |= UCRXIE;                // Enable RX interrupt
	
	__enable_irq();
	NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);           // Enable interrupts
	while(1);
}

void EUSCIB0_IRQHandler() {
    char value = UCA1RXBUF;
    if (value == '\n') {
        if (strncmp(cmdbuf, "HELLO WORLD", 11) == 0) {
            P1OUT |= BIT0;
        }
//        } else {
//            P1OUT &= ~BIT0;
//        }
        cmd_index = 0;
    } else {
        cmdbuf[cmd_index] = value;
        cmd_index++;
    }
}
