/*
 * MPU6050.c
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description:  *	Main implementation of functions to interact with
 *													the 6-dof MPU6050 Accelerometer and Gyroscope
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 */
 
#include "MPU6050.h"
#include "I2C.h"
#include "UART0.h"
#include "tm4c123gh6pm.h"
#include <stdio.h>
#include <math.h>

#define ACCEL_LSB_0_VALUE		(16384.0)
#define ACCEL_LSB_1_VALUE		(8192.0)
#define ACCEL_LSB_2_VALUE		(4096.0)
#define ACCEL_LSB_3_VALUE		(2048.0)

#define GYRO_LSB_0_VALUE		(131.0)
#define GYRO_LSB_1_VALUE		(65.5)
#define GYRO_LSB_2_VALUE		(32.8)
#define GYRO_LSB_3_VALUE		(16.4)

/*
 *	-------------------MPU6050_Init---------------------
 *	Basic Initialization Function for MPU6050 @ default settings
 *	Input: none
 * 	Output: none
 */
void MPU6050_Init(void){
	
	uint8_t ret;
	char stringBuf[10];
	
	//If check does not equal to their respected address, MPU is not detected
	#ifndef USE_HIGH
	ret = I2C_Receive(MPU6050_ADDR_AD0_LOW, WHO_AM_I);
	if(ret != MPU6050_ADDR_AD0_LOW){
		UART0_OutString("MPU6050 has not been Detected\r\n");
		return;
	}
	#else
	ret = I2C_Receive(MPU6050_ADDR_AD0_HIGH, WHO_AM_I);
	if(ret != MPU6050_ADDR_AD0_HIGH){
		UART0_OutString("MPU6050 has not been Detected\r\n");
		return;
	}
	#endif
	
	//Print ID out to terminal
	sprintf(stringBuf, "ID: %x\r\n", ret);
	UART0_OutString(stringBuf);
	
	UART0_OutString("MPU6050 has been Detected\r\n");
	UART0_OutString("MPU6050 is initializing\r\n");
	
	/* Reset the MPU6050 Module */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, PWR_MGMT_1, PWR_DEVICE_RESET);
	UART0_OutString("Reset MPU6050\r\n");
	
	/* 0 to wake up sensor */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, PWR_MGMT_1, PWR_CLK_SEL_INTERNAL);
	if(ret != 0)
		UART0_OutString("Error On Transmit\r\n");
	else
		UART0_OutString("Sensor is awake\r\n");
	
	/* Set Data Rate to 1kHz */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, SMPLRT_DIV, SMPLRT_DIV_8);
	if(ret != 0)
		UART0_OutString("Error On Transmit\r\n");
	else
		UART0_OutString("Data Rate is 1kHz\r\n");
	
	/* Default Configuration */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, CONFIG, CONFIG_DFPL_0);
	if(ret != 0)
		UART0_OutString("Error On Transmit\r\n");
	else
		UART0_OutString("Default Configuration\r\n");
	
	/* Default config for Accelerometer */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, ACCEL_CONFIG, ACCEL_AFS_SEL_0);
	if(ret != 0)
		UART0_OutString("Error On Transmit\r\n");
	else
		UART0_OutString("Default Accelerometer Configuration\r\n");
	
	/* Default config for Gyroscope */
	ret = I2C_Transmit(MPU6050_ADDR_AD0_LOW, GYRO_CONFIG, GYRO_FS_SEL_0);
	if(ret != 0)
		UART0_OutString("Error On Transmit\r\n");
	else
		UART0_OutString("Default Gyroscope Configuration\r\n");
	
	UART0_OutString("MPU6050 Initialized\r\n");
}

/*
 *	-----------------MPU6050_Get_Accel------------------
 *	Receive Raw Accelerometer Data and store it in the user struct
 *	Input: MPU6050 Accel User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Accel(MPU6050_ACCEL_t* Accel_Instance){
	
	/* Local Variables */
	uint8_t ACCEL_X_LOW;
	uint8_t ACCEL_X_HIGH;
	uint8_t ACCEL_Y_LOW;
	uint8_t ACCEL_Y_HIGH;
	uint8_t ACCEL_Z_LOW;
	uint8_t ACCEL_Z_HIGH;
	
	/* Grab 16-bit Accel data of each axis by reading ACCEL data register using I2C */
	ACCEL_X_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_XOUT_H);
	ACCEL_X_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_XOUT_L);
	ACCEL_Y_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_YOUT_H);
	ACCEL_Y_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_YOUT_L);
	ACCEL_Z_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_ZOUT_H);
	ACCEL_Z_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_ZOUT_L);

	/* Concatenate and Save Into Accelerometer Struct Instance */
	Accel_Instance->Ax_RAW = ((int16_t)(ACCEL_X_HIGH << 8) | ACCEL_X_LOW);
	Accel_Instance->Ay_RAW = ((int16_t)(ACCEL_Y_HIGH << 8) | ACCEL_Y_LOW);
	Accel_Instance->Az_RAW = ((int16_t)(ACCEL_Z_HIGH << 8) | ACCEL_Z_LOW);

	
}

/*
 *	-----------------MPU6050_Get_Gyro-------------------
 *	Receive Raw Gyroscope Data and store it in the user struct
 *	Input: MPU6050 Gyro User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Gyro(MPU6050_GYRO_t* Gyro_Instance){
		
	/* Local Variables */
	uint8_t GYRO_X_LOW;
	uint8_t GYRO_X_HIGH;
	uint8_t GYRO_Y_LOW;
	uint8_t GYRO_Y_HIGH;
	uint8_t GYRO_Z_LOW;
	uint8_t GYRO_Z_HIGH;
	
/* Grab 16-bit Gyro data of each axis by reading GYRO data register using I2C */
	GYRO_X_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_XOUT_H);
	GYRO_X_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_XOUT_L);
	GYRO_Y_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_YOUT_H);
	GYRO_Y_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_YOUT_L);
	GYRO_Z_HIGH = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_ZOUT_H);
	GYRO_Z_LOW  = I2C_Receive(MPU6050_ADDR_AD0_LOW, GYRO_ZOUT_L);

/* Concatenate and Save Into Gyroscope Struct Instance */
	Gyro_Instance->Gx_RAW = ((int16_t)(GYRO_X_HIGH << 8) | GYRO_X_LOW);
	Gyro_Instance->Gy_RAW = ((int16_t)(GYRO_Y_HIGH << 8) | GYRO_Y_LOW);
	Gyro_Instance->Gz_RAW = ((int16_t)(GYRO_Z_HIGH << 8) | GYRO_Z_LOW);
}

/*
 *	---------------MPU6050_Process_Accel----------------
 *	Process Raw Accelerometer Data into usable data and store
 *	it in the user stuct
 *	Input: MPU6050 Accel User Instance Struct
 * 	Output: none
 */
void MPU6050_Process_Accel(MPU6050_ACCEL_t* Accel_Instance){
	
	char LSB_Sensitivity;
	
	//Read LSB Sensitivity Setting from ACCEL_CONFIG Register
	#ifndef USE_HIGH
		LSB_Sensitivity = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_CONFIG);
	#else
		LSB_Sensitivity = I2C_Receive(MPU6050_ADDR_AD0_HIGH, ACCEL_CONFIG);
	#endif
	
	//Based on setting, process raw data accordingly
	switch(LSB_Sensitivity){
		case ACCEL_AFS_SEL_0:
			Accel_Instance->Ax = (float)Accel_Instance->Ax_RAW / ACCEL_LSB_0_VALUE;
			Accel_Instance->Ay = (float)Accel_Instance->Ay_RAW / ACCEL_LSB_0_VALUE;
			Accel_Instance->Az = (float)Accel_Instance->Az_RAW / ACCEL_LSB_0_VALUE;
			break;
		case ACCEL_AFS_SEL_1:
			Accel_Instance->Ax = (float)Accel_Instance->Ax_RAW / ACCEL_LSB_1_VALUE;
			Accel_Instance->Ay = (float)Accel_Instance->Ay_RAW / ACCEL_LSB_1_VALUE;
			Accel_Instance->Az = (float)Accel_Instance->Az_RAW / ACCEL_LSB_1_VALUE;
			break;
		case ACCEL_AFS_SEL_2:
			Accel_Instance->Ax = (float)Accel_Instance->Ax_RAW / ACCEL_LSB_2_VALUE;
			Accel_Instance->Ay = (float)Accel_Instance->Ay_RAW / ACCEL_LSB_2_VALUE;
			Accel_Instance->Az = (float)Accel_Instance->Az_RAW / ACCEL_LSB_2_VALUE;
			break;
		case ACCEL_AFS_SEL_3:
			Accel_Instance->Ax = (float)Accel_Instance->Ax_RAW / ACCEL_LSB_3_VALUE;
			Accel_Instance->Ay = (float)Accel_Instance->Ay_RAW / ACCEL_LSB_3_VALUE;
			Accel_Instance->Az = (float)Accel_Instance->Az_RAW / ACCEL_LSB_3_VALUE;
			break;
	}
}

/*
 *	---------------MPU6050_Process_Gyro----------------
 *	Process Raw Gyroscope Data into usable data and store it in
 *	the user struct
 *	Input: MPU6050 Gyro User Instance Struct
 * 	Output: none
 */
void MPU6050_Process_Gyro(MPU6050_GYRO_t* Gyro_Instance){
	
	char LSB_Sensitivity;
	
	//Read LSB Sensitivity Setting from GYRO_CONFIG Register
	#ifndef USE_HIGH
		LSB_Sensitivity = I2C_Receive(MPU6050_ADDR_AD0_LOW, ACCEL_CONFIG);
	#else
		LSB_Sensitivity = I2C_Receive(MPU6050_ADDR_AD0_HIGH, ACCEL_CONFIG);
	#endif
	
	//Based on setting, process raw data accordingly
	switch(LSB_Sensitivity){
		case GYRO_FS_SEL_0:
			Gyro_Instance->Gx = (float)Gyro_Instance->Gx_RAW / GYRO_LSB_0_VALUE;
			Gyro_Instance->Gy = (float)Gyro_Instance->Gy_RAW / GYRO_LSB_0_VALUE;
			Gyro_Instance->Gz = (float)Gyro_Instance->Gz_RAW / GYRO_LSB_0_VALUE;
			break;
		case GYRO_FS_SEL_1:
			Gyro_Instance->Gx = (float)Gyro_Instance->Gx_RAW / GYRO_LSB_1_VALUE;
			Gyro_Instance->Gy = (float)Gyro_Instance->Gy_RAW / GYRO_LSB_1_VALUE;
			Gyro_Instance->Gz = (float)Gyro_Instance->Gz_RAW / GYRO_LSB_1_VALUE;
			break;
		case GYRO_FS_SEL_2:
			Gyro_Instance->Gx = (float)Gyro_Instance->Gx_RAW / GYRO_LSB_2_VALUE;
			Gyro_Instance->Gy = (float)Gyro_Instance->Gy_RAW / GYRO_LSB_2_VALUE;
			Gyro_Instance->Gz = (float)Gyro_Instance->Gz_RAW / GYRO_LSB_2_VALUE;
			break;
		case GYRO_FS_SEL_3:
			Gyro_Instance->Gx = (float)Gyro_Instance->Gx_RAW / GYRO_LSB_3_VALUE;
			Gyro_Instance->Gy = (float)Gyro_Instance->Gy_RAW / GYRO_LSB_3_VALUE;
			Gyro_Instance->Gz = (float)Gyro_Instance->Gz_RAW / GYRO_LSB_3_VALUE;
			break;
	}
}

/*
 *	-----------------MPU6050_Get_Angle-----------------
 *	Calculate Tilt Angle using processed Accelerometer and
 *	Gyroscope data and it in the user angle struct
 *	Input: MPU6050 Angle User Instance Struct
 * 	Output: none
 */
void MPU6050_Get_Angle(MPU6050_ACCEL_t* Accel_Instance, MPU6050_GYRO_t* Gyro_Instance, MPU6050_ANGLE_t* Angle_Instance){
	
	//Hint: Use RAD_TO_DEGREE_CONV macro to convert radian to degree
	//Angle_Instance->ArX = atan2f(Accel_Instance->Ay, Accel_Instance->Az) * RAD_TO_DEGREE_CONV;
	//Angle_Instance->ArY = atan2f(-Accel_Instance->Ax, sqrtf(Accel_Instance->Ay * Accel_Instance->Ay + Accel_Instance->Az * Accel_Instance->Az)) * RAD_TO_DEGREE_CONV;
	float Gyro_Angle;
	Gyro_Angle += Gyro_Instance->Gz;
	
	Angle_Instance->ArX = atan2f(Accel_Instance->Ay, Accel_Instance->Az) * RAD_TO_DEGREE_CONV;
	Angle_Instance->ArY = atan2f(Accel_Instance->Ax, Accel_Instance->Az) * RAD_TO_DEGREE_CONV;
	
	float Gyro_Angle_Scale = 0.5f;
	float deadZone = 5; 
	
	if(Gyro_Instance->Gz > deadZone){
		Angle_Instance->ArZ += Gyro_Angle*Gyro_Angle_Scale; // Typically not derived from accel alone
	}
	
	
}

/* Used for Debugging Purposes */
uint8_t MPU6050_Read_Reg(uint8_t reg){
	return I2C_Receive(MPU6050_ADDR_AD0_LOW, reg);
}
