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
	
	//P6DIR |= BIT6;
	P1DIR |= BIT0;
	while (P1IN & BIT5);                   // If clock sig from mstr stays low, it is not yet in SPI mode
	flash_spi_detected();                 // Blink 3 times
	
	//FLASH SPI BITCH
//	while(1) {
//	    if(P1IN & BIT5) {
//	        P1OUT = ~P1OUT;
//	    } else if(P1IN & ~BIT5) {
//	        P1OUT = ~P1OUT;
//	    }
//	    delay(0x4fff);
//	    delay(0x4fff);
//	}

	P1SEL0 = BIT5 + BIT6 + BIT7;       // B0 clk + SIMO + SOMI
	P1SEL1 = BIT5 + BIT6 + BIT7;       // B0 clk + SIMO + SOMI

//	P6SEL0 = BIT6;              // B3SDA
//	P6SEL1 = BIT6;              // B3SDA
	
//	EUSCI_A1->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI state machine in reset
//	EUSCI_A1->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI state machine in reset
////	            EUSCI_A_CTLW0_MST |             // Set as SPI master
//	            EUSCI_A_CTLW0_SYNC |            // Set as synchronous mode
//	            EUSCI_A_CTLW0_CKPL |            // Set clock polarity high
//	            EUSCI_A_CTLW0_MSB;              // MSB first
//	EUSCI_A1->CTLW0 |= EUSCI_A_CTLW0_SSEL__ACLK; // ACLK
////	EUSCI_A1->BRW = 0x01;                   // /2,fBitClock = fBRCLK/(UCBRx+1).
//	EUSCI_A1->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;// Initialize USCI state machine

	UCB0CTLW0 |= UCSWRST;            // Enable software reset
	UCB0CTLW0 = UCSWRST | UCMSB | UCSYNC | UCCKPL;    // 3-pin, 8-bit, most sig bit, synchronous mode
	UCB0CTLW0 &= ~UCSWRST;
	
	UCB0IE |= UCRXIE;                // Enable RX interrupt
	
	__enable_irq();
	NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);           // Enable interrupts

//	  while(1) {
//	      if(UCB0STATW & UCBUSY) {
//	          P1OUT = ~P1OUT;
//	      }
//	      delay(0x4fff);
//	      delay(0x4fff);
//	  }


	while(1);
}

void EUSCIB0_IRQHandler() {
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
    UCB0IFG &= ~UCRXIFG;
}
