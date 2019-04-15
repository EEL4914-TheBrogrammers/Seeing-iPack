/*
 * spi_msp.h
 */

#ifndef SPI_MSP_H_
#define SPI_MSP_H_

#include <string.h>

/**************************************************
 *                                                *
 *                  DEFINITIONS                   *
 *                                                *
 **************************************************/
#define BUF_SIZE        (2)


/**************************************************
 *                                                *
 *              GLOBAL VARIABLES                  *
 *                                                *
 **************************************************/
char cmdbuf[BUF_SIZE];
int cmd_index;
int start;
int stop;
int phrase;
int speaking;
int cam_config;


/**************************************************
 *                                                *
 *              FUNCTION PROTOTYPES               *
 *                                                *
 **************************************************/
void spi_init();                         // SPI initialization
void spi_interrupt_init();               // SPI interrupt initialization

#endif /* SPI_MSP_H_ */
