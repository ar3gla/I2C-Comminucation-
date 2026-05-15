/*
 * WTimer.h
 *
 * Course number: CECS 447
 * Term: Spring 2026
 * Project number: Project 2
 * Project description: Implements delay and value mapping utility functions using
 *  										Wide Timer 0 prescaled to 1kHz for 1ms tick resolution
 * Team #: 7
 * Team members: Adrian Gustilo, Alfredo Regla, Gerald Calero, Paris Talebi
 *
 */
 

#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>

#define CONSTANT_FILL	(50)     // a place holder for all constants needs to be defined by students
#define CODE_FILL	(0)     // a place holder for code needs to be defined by students

/* List of Fill In Macros */
#define EN_WTIMER0_CLOCK			0x01					
#define WTIMER0_TAEN_BIT			0x01	
#define WTIMER0_TATORIS  (0x01)
#define WTIMER0_32_BIT_CFG		0x04					// configured for 32 bit timer
#define WTIMER0_PERIOD_MODE		0x00000002		// configure for periodic down-count mode
#define PRESCALER_VALUE				16000


void WTIMER0_Init(void);
void DELAY_1MS(uint32_t);
int16_t map(int16_t, int16_t, int16_t, int16_t, int16_t);

#endif
