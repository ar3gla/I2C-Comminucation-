/*
 * I2C.h
 *
 *	Provides the I2C Init, Read, and Write Function
 *
 * Created on: May 24th, 2023
 *		Author: Jackie Huynh
 *
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "wtimer.h"

/* List of Fill In Macros */

//Init Function
#define EN_I2C1_CLOCK			(SYSCTL_RCGCI2C_R1)
#define EN_GPIOA_CLOCK		(SYSCTL_RCGCGPIO_R0)
#define I2C_PINS					(0xC0)
#define I2C_ALT_FUNC_MSK	(0xFF000000)
#define I2C_ALT_FUNC_SET	(0x33000000)
#define I2C_SDA_PIN			(0x80) //PA7
#define I2C_SCL_PIN			(0x40) //PA6
#define EN_I2C_MASTER		(I2C_MCR_MFE) //bit 4
#define I2C_MTPR_TPR_VALUE	(0x07)
#define I2C_MTPR_STD_SPEED (0x00) //clearing bit 7 disables high speed

//Transmit Function (Most came from above Macros)
//#define I2C1_RW_PIN				(CONSTANT_FILL)
#define I2C1_R_PIN 				(0x01) //1 for read
#define I2C1_W_PIN				(0x00) //0 for write

//Burst Transmit Function
#define RUN_CMD						(0x01) //sets bit 0 (RUN condition) 
#define START_CMD 				(0x02) //sets bit 1 (START condition)
#define STOP_CMD					(0x04) //sets bit 2 (STOP condition)
#define ERROR_BIT_MASK		(0x02) //error bit
#define ERROR_BYTE				(0xFF) //when error occurs return 0xFF
#define BUSY_BIT					(0x01) //bit 0 (BUSY bit)
#define BUS_BUSY_BIT			(0x40) //bit 6 (BUS BUSY bit)

/*
 *	-------------------I2C1_Init------------------
 *	Basic I2C Initialization function for master mode @ 100kHz
 *	Input: None
 *	Output: None
 */
void I2C_Init(void);

/*
 *	-------------------I2C1_Receive------------------
 *	Polls to receive data from specified peripheral
 *	Input: Slave address & Slave Register Address
 *	Output: Returns 8-bit data that has been received
 */
uint8_t I2C_Receive(uint8_t slave_addr, uint8_t slave_reg_addr);

/*
 *	-------------------I2C1_Transmit------------------
 *	Transmit a byte of data to specified peripheral
 *	Input: Slave address, Slave Register Address, Data to Transmit
 *	Output: Any Errors if detected, otherwise 0
 */
uint8_t I2C_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t data);

//Has Yet to be Implemented
/*
 *	----------------I2C1_Burst_Receive-----------------
 *	Polls to receive multiple bytes of data from specified
 *  peripheral by incrementing starting slave register address
 *	Input: Slave address, Slave Register Address, Data Buffer
 *	Output: None
 */
void I2C_Burst_Receive(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size);

/*
 *	----------------I2C1_Burst_Transmit-----------------
 *	Transmit multiple bytes of data to specified peripheral
 *  by incrementing starting slave address
 *	Input: Slave address, Slave Register Address, Data Buffer to transmit, Size of Transmit
 *	Output: None
 */
uint8_t I2C_Burst_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size);

#endif //I2C_H_
