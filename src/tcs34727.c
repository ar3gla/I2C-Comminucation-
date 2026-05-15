/*
 * TCS34727.c
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description: Main implementation of the functions to interact with
 *											the TCS34727 RGB Color Sensor
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 */

#include "TCS34727.h"
#include "I2C.h"
#include "UART0.h"
#include "WTimer.h"
#include <stdio.h>
#include "tm4c123gh6pm.h"

/*	-------------------TCS34727_Init------------------
 *	Basic Initialization Function for TCS34727 at default settings
 *	Input: none
 *	Output: none
 */
void TCS34727_Init(void){
	uint8_t ret;																//Temp Variable to hold return values
	char printBuf[20];													//String buffer to print
	
	/* Check if RGB Color Sensor has been detected */
	ret = I2C_Receive(TCS34727_ADDR, TCS34727_CMD|TCS34727_ID_R_ADDR);
	
	//Print ID or Error to Terminal
	sprintf(printBuf, "ID: %x\r\n", ret);
	UART0_OutString(printBuf);
	
	if(ret != TCS34727_ID){
		UART0_OutString("TCS34727 has not been Detected\r\n");
		return;
	}
	UART0_OutString("TCS34727 has been Detected\r\n");
	
	/* Set Integration Time to 2.4ms in timing register */
	ret = I2C_Transmit(TCS34727_ADDR, TCS34727_CMD|TCS34727_TIMING_R_ADDR, TCS34727_ATIME_2_4_MS);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34727 Integration Time Set\r\n");
	
	// Necessary Delay when setting integration time/wait time. 
	// This varies for which integration time is choosen.
	// This project chooses 2.4ms.
	DELAY_1MS(3);
	
	/* Setting Gain to 1X gain */
	ret = I2C_Transmit(TCS34727_ADDR, TCS34727_CMD|TCS34727_CTRL_R_ADDR, TCS34727_CTRL_AGAIN_1);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34727 Gain Set\r\n");
	
	/* Powering On Sensor at Enable register */
	ret = I2C_Transmit(TCS34727_ADDR, TCS34727_CMD|TCS34727_ENABLE_R_ADDR, TCS34727_ENABLE_PON);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34727 Power On\r\n");

	//Nessessary Delay When Powering On Module
	DELAY_1MS(3);
	
	/* Enabling RGBC 2-Channel ADC at Enable register */
	ret = I2C_Transmit(TCS34727_ADDR, TCS34727_CMD|TCS34727_ENABLE_R_ADDR, TCS34727_ENABLE_PON |TCS34727_ENABLE_AEN);
	if(ret != 0)
		UART0_OutString("Error on Transmit\r\n");
	else
		UART0_OutString("TCS34727 RGBC On\r\n");
	
	//Integration Time Delay when Activating. Varies with Integration Time Choosen by User
	DELAY_1MS(3);
	
	UART0_OutString("TCS34727 Color Sensor Initialized\r\n");
	
}

/*	---------------TCS34727_GET_RAW_CLEAR-------------
 *	Receive RAW clear data reading from the sensor
 *	Input: none
 *	Output: Returns 16-bit RAW clear data
 */
uint16_t TCS34727_GET_RAW_CLEAR(void){
	uint8_t CLEAR_LOW;
	uint8_t CLEAR_HIGH;
	uint16_t CLEAR_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	CLEAR_LOW = I2C_Receive(TCS34727_ADDR, TCS34727_CMD|TCS34727_CDATAL_R_ADDR);
	CLEAR_HIGH = I2C_Receive(TCS34727_ADDR, TCS34727_CMD|TCS34727_CDATAH_R_ADDR);
	
	/* Concatanate into 16-bit value */
	CLEAR_DATA = ((uint16_t)CLEAR_HIGH << 8 ) | CLEAR_LOW;
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return CLEAR_DATA;
}

/*	---------------TCS34727_GET_RAW_RED---------------
 *	Receive RAW red data reading from the sensor
 *	Input: none
 *	Output: Returns 16-bit RAW red data
 */
uint16_t TCS34727_GET_RAW_RED(void){
	uint8_t RED_LOW;
	uint8_t RED_HIGH;
	uint16_t RED_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	RED_LOW = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_RDATAL_R_ADDR);
	RED_HIGH = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_RDATAH_R_ADDR);

	
	/* Concatanate into 16-bit value */
	RED_DATA = ((uint16_t)RED_HIGH << 8) | RED_LOW;
	//Integration Time Delay
	DELAY_1MS(3);
	
	return RED_DATA;
}

/*	---------------TCS34727_GET_RAW_GREEN-------------
 *	Receive RAW green data reading from the sensor
 *	Input: none
 *	Output: Returns 16-bit RAW green data
 */
uint16_t TCS34727_GET_RAW_GREEN(void){
	uint8_t GREEN_LOW;
	uint8_t GREEN_HIGH;
	uint16_t GREEN_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	GREEN_LOW = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_GDATAL_R_ADDR);
	GREEN_HIGH = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_GDATAH_R_ADDR);


	
	/* Concatanate into 16-bit value */
	GREEN_DATA = ((uint16_t)GREEN_HIGH << 8) | GREEN_LOW;

	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return GREEN_DATA;
}

/*	---------------TCS34727_GET_RAW_BLUE-------------
 *	Receive RAW blue data reading from the sensor
 *	Input: none
 *	Output: Returns 16-bit RAW blue data
 */
uint16_t TCS34727_GET_RAW_BLUE(void){
	uint8_t BLUE_LOW;
	uint8_t BLUE_HIGH;
	uint16_t BLUE_DATA;
	
	/* Use I2C to grab both HIGH and LOW data */
	BLUE_LOW = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_BDATAL_R_ADDR);
	BLUE_HIGH = I2C_Receive(TCS34727_ADDR, TCS34727_CMD | TCS34727_BDATAH_R_ADDR);

	
	/* Concatanate into 16-bit value*/
	BLUE_DATA = ((uint16_t) BLUE_HIGH << 8) | BLUE_LOW;
	
	//Integration Time Delay
	DELAY_1MS(3);
	
	return BLUE_DATA;
}

/*	---------------TCS34727_GET_RGB------------------
 *	Normalize RAW data into RGB range (0-255)
 *	Input: RGB Color Struct User Instance
 *	Output: none
 */
void TCS34727_GET_RGB(RGB_COLOR_HANDLE_t* RGB_COLOR_Instance){
	
	/* Prevent Dividing by 0 by checking if the C_RAW value from struct is equal to 0 */
	if(RGB_COLOR_Instance->C_RAW == 0){
		RGB_COLOR_Instance->R = RGB_COLOR_Instance->G = RGB_COLOR_Instance->B = 0;
		return;
	}
	
	/*
	Divide all RGB value with their (RAW Value / (float)Clear Raw Value) and multiple everything with 255.0
	Store in RGB Color Instance Struct
	*/ 
	RGB_COLOR_Instance->R = (uint8_t)(((float)RGB_COLOR_Instance->R_RAW / RGB_COLOR_Instance->C_RAW) * 255.0f);
	RGB_COLOR_Instance->G = (uint8_t)(((float)RGB_COLOR_Instance->G_RAW / RGB_COLOR_Instance->C_RAW) * 255.0f);
	RGB_COLOR_Instance->B = (uint8_t)(((float)RGB_COLOR_Instance->B_RAW / RGB_COLOR_Instance->C_RAW) * 255.0f);

	
}

/*	-----------------Detect_Color--------------------
 *	Detect which color is more prominant and returns that color
 *	Input: RGB Color User Instance Struct
 *	Output: COLOR_DETECTED enum value
 */
COLOR_DETECTED Detect_Color(RGB_COLOR_HANDLE_t* RGB_COLOR_Instance){
	
	/* Compare all values with eachother and return which color is prominent using enum type */
	if(RGB_COLOR_Instance->R > RGB_COLOR_Instance->G && RGB_COLOR_Instance->R > RGB_COLOR_Instance->B)
		return RED_DETECT;
	else if(RGB_COLOR_Instance->G > RGB_COLOR_Instance->R && RGB_COLOR_Instance->G > RGB_COLOR_Instance->B)
		return GREEN_DETECT;
	else if(RGB_COLOR_Instance->B > RGB_COLOR_Instance->R && RGB_COLOR_Instance->B > RGB_COLOR_Instance->G)
		return BLUE_DETECT;
	
	/* Otherwise no color is being detected */
	return NOTHING_DETECT;
}
