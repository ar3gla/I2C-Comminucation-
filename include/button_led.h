/*
 * ButtonLED.h
 *
 *	Provides function to initialize the onboard RGB LED and
 *	buttons on the TIVA TM4C as well as macros for the different
 *	possible color combination 
 *
 * Created on: March 1st, 2023
 *		Author: ALfredo Regla
 *
 */
 
#include "tm4c123gh6pm.h"
#include <stdint.h>

#ifndef BUTTONLED_H_
#define BUTTONLED_H_

#define BUTTONS					0x11

#define LED_PINS				0x0E
#define DARK    				0x00
#define RED   					0x02
#define BLUE    				0x04
#define GREEN  					0x08
#define YELLOW  				0x0A
#define CYAN   					0x0C
#define WHITE   				0x0E
#define PURPLE 					0x06

/* Comment out if BTN interrupt is not needed */
#define USE_BTN_INTERRUPT

#define NVIC_EN0_PORTF 	0x40000000
#define LEDs (*((volatile unsigned long *)0x40025038))  // use onboard three LEDs: PF321

#define SW1_FLAG 				GPIO_PORTF_MIS_R & 0x10
#define SW2_FLAG 				GPIO_PORTF_MIS_R & 0x01
#define SW1_PIN					0x10
#define SW2_PIN					0x01
#define PORTF_FLAGS			GPIO_PORTF_ICR_R

void LED_Init(void);
void BTN_Init(void);
#endif


