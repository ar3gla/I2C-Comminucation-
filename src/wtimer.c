/*
 * WTimer.c
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
 
#include "WTimer.h"
#include "tm4c123gh6pm.h"

/* Local Macros */
#define TIMER_32_MAX_RELOAD		(4294967295)	
 
/* The reason why Wide Timer is used instead of regular time is because
	 of the prescaler option */
void WTIMER0_Init(void)
{
	SYSCTL_RCGCWTIMER_R |= EN_WTIMER0_CLOCK;						//Enable WTIMER0 Clock
	
	//Wait Until WTIMER0 Clock has be activated
	while((SYSCTL_RCGCWTIMER_R&EN_WTIMER0_CLOCK)!=EN_WTIMER0_CLOCK);
	
	WTIMER0_CTL_R &= ~(WTIMER0_TAEN_BIT);									//Disable WTIMER0 Timer A
	WTIMER0_CFG_R |= WTIMER0_32_BIT_CFG;								//Set WTIMER0 to be 32-bit config mode
	WTIMER0_TAMR_R |= WTIMER0_PERIOD_MODE;							//Set WTIMER0 to be in periodic mode
	
	/* Prescale down to 1MHz or 1us period */
	// Frequency = System Clock / Prescaler
	// Frequency = 40MHz / 40000 = 0.001MHz = 1kHz
	// Tick Length = Period = 1 / 1kHz = 1ms
	WTIMER0_TAPR_R = PRESCALER_VALUE;										//Set prescaler to get 1kHz frequency or 1ms period
	
	// 1Khz = 16 MHz / x 
	// x = 16 Mhz / 2Hz = 8 MHz
	//
}

void DELAY_1MS(uint32_t delay) 
{
    WTIMER0_TAILR_R = delay;           // load count (already in ms ticks)
    WTIMER0_ICR_R  |= WTIMER0_TATORIS; // clear any stale timeout flag first
    WTIMER0_CTL_R  |= WTIMER0_TAEN_BIT;// start timer
    while ((WTIMER0_RIS_R & WTIMER0_TATORIS) == 0) {}; // wait for timeout
    WTIMER0_CTL_R  &= ~(WTIMER0_TAEN_BIT); // stop timer
    WTIMER0_ICR_R  |= WTIMER0_TATORIS;     // clear flag
}

int16_t map(int16_t x, int16_t x_min, int16_t x_max, int16_t out_min, int16_t out_max){
	if(x < x_min){
		return x_min;
	}
	if(x > x_max){
		return x_max;
	}
	return (x - x_min) * (out_max - out_min) / (x_max - x_min) + out_min;
}
