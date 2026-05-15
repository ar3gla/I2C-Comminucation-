/*
 * Servo.c
 *
 *	Implementations of functions to interact with a standard Servo Motor
 *
 * Created on: June 11th, 2023
 *
 */
 
#include "Servo.h"
#include "util.h"
#include "tm4c123gh6pm.h"

/*
 *	-------------------Servo_Init------------------
 *	Basic Servo Initialization function for PWM Generation
 *	Input: None
 *	Output: None
 */
void Servo_Init(void){
	SYSCTL_RCGC2_R |= EN_PWM0_GPIOB_CLOCK;				//Activate GPIOB Clock
	while((SYSCTL_RCGC2_R&EN_PWM0_GPIOB_CLOCK)!=EN_PWM0_GPIOB_CLOCK);
	
	/* GPIO Configuration */
	GPIO_PORTB_AFSEL_R 	|= PWM0_PIN;							//Enable Alternate Function on PB6
	GPIO_PORTB_PCTL_R 	&= ~CLEAR_ALT_FUNCTION;		//Clear any Previous Alternate Function
	GPIO_PORTB_PCTL_R 	|= PWM0_ALT_FUNCTION;			//Set Alternate Function to PWM on PB6
	GPIO_PORTB_AMSEL_R 	&= ~(PWM0_PIN);						//Disable Analog Function
	GPIO_PORTB_DEN_R 		|= PWM0_PIN;							//Enable Digital I/O on PB6
	GPIO_PORTB_DR8R_R 	|= PWM0_PIN;							//Enable 8mA Drive
	
	/* PWM Clock Configuration */
	SYSCTL_RCGCPWM_R |= EN_PWM0_CLOCK;						//Activate PWM0 Module
	
	/*
	Servo Signal Requires 50Hz or 20ms Period
	Default 16MHz and a 16-bit counter won't be enough
	Will Need to Prescale the PWM Clock
	
	Example: 
	PWM Clock = System Clock / Prescaler = 40MHz / 16 = 2.5MHz
	Time Per Tick = 1 / PWM Clock = 1 / 2.5MHz = 0.4us
	Period = Counter * Time Per Tick
	Counter = Period / Time Per Tick = 20ms / 0.4us = 50000 - 1 = 49999
	Frequency = 1 / Period = 1 / 20ms = 50Hz
	*/
	SYSCTL_RCC_R |= EN_USE_PWM_DIV;				//Enable PWM Divider
	SYSCTL_RCC_R &= ~CLEAR_PWM_DIV; 			//Clear PWM divider field
	SYSCTL_RCC_R |= PWM0_DIV_VALUE;				//Set PWM Divider to 8
	
	/* PWM Gen 0 Configuration */
	PWM0_0_CTL_R = PWM0_DEFAULT_CONFIG;		//Default mode of count-down and auto-reload
	
	PWM0_0_GENA_R |= PWM0_GEN_CONFIG;			//low on LOAD, high on CMPA match				
	PWM0_0_LOAD_R  = PWM0_COUNTER-1;			//Set PWM counter for period
	PWM0_0_CMPA_R  = 0;										//Set to 0% duty cycle
	PWM0_0_CTL_R 	|= PWM0_START;					//Start PWM0 Gen 0
	PWM0_ENABLE_R |= EN_PWM0_FUNCTION;		//Enable PWM0
	
}

/*
 *	-------------------Drive_Servo------------------
 *	Drives the Servo Motor to a specified angle
 *	Input: Desired Angle
 *	Output: None
 */
void Drive_Servo(int16_t angle){
	
	/* Temp Variable to Hold value that is being mapped*/
	int16_t mapped;
	
	/* Assert Parameter */
	if((angle < SERVO_MIN_ANGLE) || (angle > SERVO_MAX_ANGLE))
		return;
	
	/* Map Function */
	mapped = map(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_CNT-1, SERVO_MAX_CNT-1);
	
	/* Setting New Compare Value */
	PWM0_0_CMPA_R = mapped;
	
}
