/*
 * I2C.c
 *
 *	Main implementation of the I2C Init, Read, and Write Function
 *
 * Created on: May 24th, 2023
 *		Author: Jackie Huynh
 *
 */
 
#include "I2C.h"
#include "tm4c123gh6pm.h"

/*
 *	-------------------I2C1_Init------------------
 *	Basic I2C Initialization function for master mode @ 100kHz
 *	Input: None
 *	Output: None
 */
void I2C_Init(void){
	
	/* Enable Required System Clock */
	//Enable I2C1 System Clock
	SYSCTL_RCGCI2C_R |= EN_I2C1_CLOCK; //activate I2C1
	//Enable GPIOA System Clock
	SYSCTL_RCGCGPIO_R |= EN_GPIOA_CLOCK; //activate port A
	
	//Wait Until GPIOx System Clock is enabled
	while((SYSCTL_RCGCI2C_R&EN_I2C1_CLOCK)==0){}; //wait for I2C1
	while((SYSCTL_RCGCGPIO_R&EN_GPIOA_CLOCK)==0){}; //wait for portA 
	
	
	/* GPIOx I2C Alternate Function Setup	*/
	//Enable Digital I/O
	GPIO_PORTA_DEN_R |= I2C_PINS; //enable PA7-6
	//Enable Alternate Function Selection
	GPIO_PORTA_AFSEL_R |= I2C_PINS; //enable alternate function on PA7-6
	
	//Select I2C1 as the alternate function 	
	GPIO_PORTA_PCTL_R &= ~I2C_ALT_FUNC_MSK; //clear port select
	GPIO_PORTA_PCTL_R |= I2C_ALT_FUNC_SET; //set port select to I2C1
	//Enable Open Drain for SDA pin
	GPIO_PORTA_ODR_R |= I2C_SDA_PIN; //PA7 data pin
	//Disable Analog Mode
	GPIO_PORTA_AMSEL_R &= ~I2C_PINS;
		
	//(Optional) enable PORTA Pull-up resistor
	//GPIO_PORTA_PUR_R |= I2C_PINS; //enable pull-up resistors for PA7-6 so resistors aren't needed in the circuit
	
	/*	I2C1 Setup as Master Mode @ 100kBits	*/
	//Configure I2C1 as Master 
	I2C1_MCR_R |= EN_I2C_MASTER; //master function enable
	
	/* Configuring I2C Clock Frequency to 100KHz
		
		TPR = (System Clock / (2*(SCL_LP + SCL_HP) * SCL_CLK)) - 1
		SCL_LP and SCL_HP are fixed
		SCL_LP = 6 & SCL_HP = 4
		
		Example if we want to configure I2C speed to 100kHz for 40MHz system clock
		TPR = (40MHz / ((2*(6+4)) * 100kHz)) - 1 		(Convert Everything to Hz)
		TPR = 19
		
	*/
	
	// take care of master timer period: standard speed and TPR value	
	I2C1_MTPR_R = I2C_MTPR_TPR_VALUE; //TPR = (16Mz / ((2*(6+4)) * 100kHz)) - 1 = 7

}

/*
 *	-------------------I2C1_Receive------------------
 *	Polls to receive data from specified peripheral
 *	Input: Slave address & Slave Register Address
 *	Output: Returns 8-bit data that has been received
 */
uint8_t I2C_Receive(uint8_t slave_addr, uint8_t slave_reg_addr){
	
	char error;																	//Temp Variable to hold errors
	
	/* Check if I2C1 is busy: check MCS register Busy bit */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	/* Configure I2C1 Slave Address and Read Mode */
	// Slave Address is the 7 MSB
	I2C1_MSA_R = (slave_addr << 1) | I2C1_W_PIN; //left shift upper 7 bits of slave address, and set write mode which is 0
	// Set Data Register to slave register address
	I2C1_MDR_R = slave_reg_addr;
	
	/* Initiate I2C by generating a START & RUN cmd:
	   Set MCS register START bit to generate and RUN bit to enable I2C Master
	*/
	I2C1_MCS_R = (RUN_CMD + START_CMD); //sets bit 0 (RUN condition) and bit 1 (START condition)
	
	/* Wait until write is done: check MCS register to see is I2C is still busy */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	
	/* Set I2C to Receive with Slave Address and change to Read */
	I2C1_MSA_R = (slave_addr << 1) | I2C1_R_PIN; //left shift upper 7 bits of slave address, and set read mode which is 1
	
	/* Initiate I2C by generating a repeated START, STOP, & RUN cmd */
	I2C1_MCS_R = (RUN_CMD + START_CMD + STOP_CMD); //sets bit 0 (RUN condition) and bit 1 (START condition) and bit 2 (STOP condition)
	
	/* Wait until I2C bus is not busy: check MCS register for I2C bus busy bit */
	while(I2C1_MCS_R & BUS_BUSY_BIT){}; //poll for bus busy bit (bit 6) to equal 0
	
	/* Check for any error: read the error flag from MCS register */
	error = I2C1_MCS_R & ERROR_BIT_MASK; //check for error
	if(error){
		return ERROR_BYTE; //return 0xFF to indicate error
	}
		
	return I2C1_MDR_R & 0xFF; //return DR masked with 8 bits
}

/*
 *	-------------------I2C1_Transmit------------------
 *	Transmit a byte of data to specified peripheral
 *	Input: Slave address, Slave Register Address, Data to Transmit
 *	Output: Any Errors if detected, otherwise 0
 */
uint8_t I2C_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t data){
	
	char error;																	//Temp Variable to hold errors
	
	/* Check if I2C1 is busy: check MCS register Busy bit */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	/* Configure I2C Slave Address, R/W Mode, and what to transmit */
	//Slave Address is the first 7 MSB
	// Clear LSB to write
	I2C1_MSA_R = (slave_addr << 1) | I2C1_W_PIN; //left shift upper 7 bits of slave address, and set write mode which is 0
	//Transmit register addr to interact
	I2C1_MDR_R = slave_reg_addr;
	
	/* Initiate I2C by generate a START bit and RUN cmd */
	I2C1_MCS_R = (RUN_CMD + START_CMD); //sets bit 0 (RUN condition) and bit 1 (START condition)
	/* Wait until write has been completed */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	/* Update Data Register with data to be transmitted */
	I2C1_MDR_R = data;
	/* Initiate I2C by generating a STOP & RUN cmd */
	I2C1_MCS_R = (RUN_CMD + STOP_CMD); //sets bit 0 (RUN condition) and bit 2 (STOP condition)
	/* Wait until write has been completed: check MCS register Busy bit */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	/* Wait until bus isn't busy: check MCS register for I2C bus busy bit */
	while(I2C1_MCS_R & BUS_BUSY_BIT){}; //poll for bus busy bit (bit 6) to equal 0
	/* Check for any error: read the error flag from MCS register */
	error = I2C1_MCS_R & ERROR_BIT_MASK; //check for error
	if(error){
		return ERROR_BYTE; //return 0xFF to indicate error
	}
	return 0; //0 here indicates success
}

//Has Yet to be Implemented
/*
 *	----------------I2C1_Burst_Receive-----------------
 *	Polls to receive multiple bytes of data from specified
 *  peripheral by incrementing starting slave register address
 *	Input: Slave address, Slave Register Address, Data Buffer
 *	Output: None
 */
void I2C_Burst_Receive(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size){
	
}

/*
 *	----------------I2C1_Burst_Transmit-----------------
 *	Transmit multiple bytes of data to specified peripheral
 *  by incrementing starting slave address
 *	Input: Slave address, Slave Register Address, Data Buffer to transmit
 *	Output: None
 */
uint8_t I2C_Burst_Transmit(uint8_t slave_addr, uint8_t slave_reg_addr, uint8_t* data, uint32_t size){
	
	char error;															//Temp Error Variable
	
	/* Asserting Param */
	if(data == 0 || size == 0){ //check if array is empty or size is 0
		return 0xFF;
	}
	
	/* Check if I2C1 is busy */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	
	/* Configure I2C Slave Address, R/W Mode, and what to transmit */
	//Slave Address is the first 7 MSB
	//Clear LSB to write
	I2C1_MSA_R = (slave_addr << 1) | I2C1_W_PIN; //left shift upper 7 bits of slave address, and set write mode which is 0
	//Transmit register addr to interact
	I2C1_MDR_R = slave_reg_addr;
	
	/* Initiate I2C by generate a START bit and RUN cmd */
	I2C1_MCS_R = (RUN_CMD + START_CMD); //sets bit 0 (RUN condition) and bit 1 (START condition)
	/* Wait until write has been completed */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	/* Loop to Burst Transmit what is stored in data buffer */
	while(size > 1){
		
		//Deference Pointer from data array and load into data reg. Post-Increment the pointer after
		I2C1_MDR_R = *data++;
		//Initiate I2C RUN CMD
		I2C1_MCS_R = RUN_CMD; //sets bit 0 (RUN condition) 
		//Wait until transmit is complete
		while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
		size--;																//Reduce size until 1 is left
		
	}
	
	//Deference Pointer from data array and load into data reg
	//Initiate I2C STOP condition and RUN CMD
	I2C1_MDR_R = *data++;
	/* Wait until write has been completed */
	I2C1_MCS_R = (RUN_CMD + STOP_CMD); //sets bit 0 (RUN condition) and bit 2 (STOP condition)
	/* Wait until bus isn't busy */
	while(I2C1_MCS_R & BUSY_BIT){}; //poll for busy bit (bit 0) to equal 0, which indicates it is not busy
	while(I2C1_MCS_R & BUS_BUSY_BIT){}; //poll for bus busy bit (bit 6) to equal 0
	/* Check for any error */
	error = I2C1_MCS_R & ERROR_BIT_MASK; //check for error
	if(error){
		return ERROR_BYTE; //return 0xFF to indicate error
	}
	return 0;
}
