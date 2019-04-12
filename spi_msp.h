/*
 * spi_msp.h
 */

#ifndef SPI_MSP_H_
#define SPI_MSP_H_

#include <string.h>

/**************************************************
 *                                                *
 *                  Definitions                   *
 *                                                *
 **************************************************/
#define BUF_SIZE        (2)


/**************************************************
 *                                                *
 *              Global Variables                  *
 *                                                *
 **************************************************/
char cmdbuf[BUF_SIZE];
int cmd_index;
int start;


/**************************************************
 *                                                *
 *              Function Prototypes               *
 *                                                *
 **************************************************/
void spi_init();                         // SPI initialization
void spi_interrupt_init();               // SPI interrupt initialization

#endif /* SPI_MSP_H_ */
