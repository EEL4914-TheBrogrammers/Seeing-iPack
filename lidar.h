#ifndef LIDAR_H_
#define LIDAR_H_

/**************************************************
 *                                                *
 *                  Definitions                   *
 *                                                *
 **************************************************/
// Registers
#define ACQ_COMMAND         (0x00)  // Device command
#define STATUS              (0x01)  // System status
#define SLAVE_ADDR          (0x62)  // LiDAR slave address
#define DISTANCE            (0x8f)  // Distance measurement

// Register commands
#define RESET               (0x00)  // Reset registers to default
#define START_MEASURE       (0x04)  // Start taking distance measurement

#define SAMPLES             (1)    // Total amount of samples


/**************************************************
 *                                                *
 *              Global Variables                  *
 *                                                *
 **************************************************/
short tx_addr;      // Address to transmit in interrupt
short tx_data;      // Data to transmit in interrupt

short lvalue;       // Distance measurement low byte
short hvalue;       // Distance measurement high byte

int counter;        // Counter for inside interrupt
int avg_count;      // Counter to calculate avg distance
float sum;          // Sum of distances to calculate avg distance


/**************************************************
 *                                                *
 *              Function Prototypes               *
 *                                                *
 **************************************************/
void i2c_init();                            // I2C initialization function
void i2c_tx(short address, short data);     // I2C transmit function
unsigned short i2c_rx(short address);       // I2C receive function

void manual_reset();                        // Reset to let slave release SDA line
void delay_ms(unsigned int delay);          // Delay function in ms

float get_distance(char* type);             // Get distance reading from LiDAR


#endif /* LIDAR_H_ */
