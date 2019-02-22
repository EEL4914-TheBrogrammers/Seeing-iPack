#include "msp.h"
#include <string.h>

/**************************************************
 *                                                *
 *                  Definitions                   *
 *                                                *
 **************************************************/
#define ACQ_COMMAND         (0x00)  // Device command
#define STATUS              (0x01)  // System status
#define DISTANCE            (0x8f)  // Distance measurement
#define FULL_DELAY_HIGH     (0x0f)  // Distance measurement high byte
#define FULL_DELAY_LOW      (0x10)  // Distance measurement low byte

#define SLAVE_ADDR          (0x62)  // LiDAR slave address
#define WRITE_ADDR          (0xC4)  // LiDAR write address
#define READ_ADDR           (0xC5)  // LiDAR read address

char *tx_type_1 = "TX_2_TX";
char *tx_type_2 = "TX_2_RX";
char *RX_STAT = "RX_stat";
char *RX_DIST = "RX_dist";


/**************************************************
 *                                                *
 *              Global Variables                  *
 *                                                *
 **************************************************/
short tx_addr;
short tx_data;
char *step;
char *rx_step;
int rx_status_complete;

short lbyte;
short hbyte;
short stat;

int tx_complete;

int counter;
int num_times_tx_called;
int num_of_rx;
int type_rx;
int write;
int turn;


/**************************************************
 *                                                *
 *                  Functions                     *
 *                                                *
 **************************************************/

void i2c_tx(short address, short data, int write_flag){
    num_times_tx_called++;
    // Set slave address register and data address
    tx_addr = address;
    tx_data = data;

    if (write_flag == 0) {
        step = "TX_2_RX";
    } else if (write_flag == 1) {
        step = "TX_2_TX";
    }

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR + EUSCI_B_CTLW0_TXSTT; // Transmit start condition
    __sleep();

}

void i2c_rx(char *rx_type){
    if (strcmp(rx_type, "RX_STAT") == 0){
        rx_step = "RX_STAT";
    } else if (strcmp(rx_type, "RX_DIST") == 0){
        rx_step = "RX_DIST";
    }

    EUSCI_B0->CTLW0 &= ~UCTR;
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // Transmit start condition
    __sleep();
}

void main(void)
{
    volatile uint32_t i;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;	// stop watchdog timer

	counter = 0;
	num_times_tx_called  = 0;
	turn = 0;
	hbyte = 0;
	lbyte = 0;
	num_of_rx = 0;
	tx_complete = 0;
	rx_status_complete = 0;
	step = "TX_2_TX";

	P1->SEL0 |= BIT6 | BIT7;        // P1.6(SDA), P1.7(SCL)

    // Enable global interrupt
    __enable_irq();

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST;     // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST |     // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |              // I2C mode
            EUSCI_B_CTLW0_MST |                 // Master mode
            EUSCI_B_CTLW0_SYNC |                // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;          // SMCLK
    EUSCI_B0->BRW = 7;                          // baudrate = SMCLK / 7 = 400kHz
    EUSCI_B0->I2CSA = SLAVE_ADDR;               // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Release eUSCI from reset

    EUSCI_B0->IE |= EUSCI_B_IE_TXIE |       // Enable transmit interrupt
            EUSCI_B_IE_NACKIE |             // Enable NACK interrupt
            EUSCI_B_IE_RXIE;                // Enable receive interrupt

    while (1) {
        // Don't wake up on exit from ISR
        SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;

        // Ensures SLEEPONEXIT takes effect immediately
        __DSB();

        // Delay between transmissions
        for (i = 1000; i > 0; i--);

        counter = 0;

        // Ensure stop condition got sent
        while (EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTP);

        if (strcmp(step, tx_type_1) == 0  && rx_status_complete == 0) {
            i2c_tx(ACQ_COMMAND, 0x04, 1);
        } else if (strcmp(step, tx_type_2) == 0 && tx_complete == 0  && rx_status_complete == 0) {
            i2c_tx(STATUS, 0x00, 0);
            tx_complete = 1;
        } else if (tx_complete == 1  && rx_status_complete == 0) {
            i2c_rx("RX_STAT");
            tx_complete = 2;
        } else if (tx_complete == 2) {
            i2c_tx(DISTANCE, 0x00, 0);
            tx_complete = 0;
            rx_status_complete = 1;
        } else if (strcmp(rx_step, "RX_DIST") == 0 && rx_status_complete == 1){
            i2c_rx("RX_DIST");
            rx_status_complete = 200;
        }

        if (lbyte != 0) {
            int temp;
            temp = 0;
        }

        // Go to LPM0
//        __sleep();
        __no_operation();                   // for debug
    }
}

// I2C interrupt service routine
void EUSCIB0_IRQHandler(void)
{
    if (EUSCI_B0->IFG & EUSCI_B_IFG_NACKIFG) {
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_NACKIFG;
        // I2C start condition
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0) {
        // Clear TX interrupt flag
        EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG0;

        if (counter == 0) {             // Transfer register address
            EUSCI_B0->TXBUF = tx_addr;
            if (strcmp(step, tx_type_1) == 0) {          // TX address & data
                counter++;
            } else if (strcmp(step, tx_type_2) == 0) {   // TX addess only
                step = "RX_DIST";
                counter = 2;
            }
        } else if (counter == 1) {      // Transfer register data
            EUSCI_B0->TXBUF = tx_data;
            step = "TX_2_RX";
            counter++;
        } else if (counter == 2) {      // Generate stop bit
            // I2C stop condition
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

            // Clear USCI_B0 TX int flag
            EUSCI_B0->IFG &= ~EUSCI_B_IFG_TXIFG;
            counter = 0;

            // Wake up on exit from ISR
            SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;

            // Ensures SLEEPONEXIT takes effect immediately
            __DSB();
        }
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0) {
        if (strcmp(rx_step, "RX_STAT") == 0) {
            stat = EUSCI_B0->RXBUF;
            while (stat & 0x01) {
                stat = EUSCI_B0->RXBUF;
            }

            // Clear RX interrupt flag
            EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;

            rx_step = "RX_DIST";

            // Wake up on exit from ISR
            SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;

            // Ensures SLEEPONEXIT takes effect immediately
            __DSB();
        } else if (strcmp(rx_step, "RX_DIST") == 0) {
            if (num_of_rx == 0) {
                hbyte = EUSCI_B0->RXBUF;
                num_of_rx ++;
            } else if (num_of_rx == 1) {
                lbyte = EUSCI_B0->RXBUF;
                num_of_rx = 0;

                // Clear RX interrupt flag
                EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;

                num_of_rx = 0;

                // Wake up on exit from ISR
                SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk;

                // Ensures SLEEPONEXIT takes effect immediately
                __DSB();
            }
        }
    }
}
