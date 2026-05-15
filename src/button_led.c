/*
 * ButtonLED.c
 *
 * Created on: March 1st, 2023
 *		Author: Alfredo Regla
 *
 */
 
#include "ButtonLED.h"

void LED_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;     												// activate F clock
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOF)!=SYSCTL_RCGC2_GPIOF){} // wait for the clock to be ready
		 
	GPIO_PORTF_CR_R 		|= LED_PINS;         													// allow changes to PF3-1     
  GPIO_PORTF_AMSEL_R 	&= ~(LED_PINS);        												// disable analog function
  GPIO_PORTF_PCTL_R 	&= ~(0x0000FFF0); 														// GPIO clear bit PCTL  
	GPIO_PORTF_DIR_R 		|= LED_PINS;          												// PF3, PF2, PF1 LED Output  
	GPIO_PORTF_AFSEL_R 	&= ~(LED_PINS);        												// no alternate function
	GPIO_PORTF_PUR_R 		&= ~(LED_PINS);          											// disable pullup resistors on PF3, PF2, PF1 		
  GPIO_PORTF_DEN_R 		|= LED_PINS;          												// enable digital pins PF3-PF1 
}

void BTN_Init(void){
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;     												// activate F clock
	while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOF)!=SYSCTL_RCGC2_GPIOF){} // wait for the clock to be ready
		
  GPIO_PORTF_LOCK_R 	 = 0x4C4F434B;   															// unlock PortF PF0  
	GPIO_PORTF_CR_R 		|= BUTTONS;         													// allow changes to PF4 & PF0     
  GPIO_PORTF_AMSEL_R 	&= ~(BUTTONS);        												// disable analog function
  GPIO_PORTF_PCTL_R 	&= ~(0x000F000F); 														// GPIO clear bit PCTL  
	GPIO_PORTF_DIR_R 		&= ~BUTTONS;          												// PF4 & PF0 as Inputs  
	GPIO_PORTF_AFSEL_R 	&= ~(BUTTONS);        												// no alternate function
	GPIO_PORTF_PUR_R 		|= BUTTONS;          													// enable pullup resistors on PF4 & PF0		
  GPIO_PORTF_DEN_R 		|= BUTTONS;          													// enable digital pins PF4 & PF0
	
	#ifdef USE_BTN_INTERRUPT
	GPIO_PORTF_IS_R 		&= ~(BUTTONS);     														// enable edge sensitive for PF4 & PF0
  GPIO_PORTF_IBE_R 		&= ~(BUTTONS);    														// disable both edge sensitive for PF4 & PF0
  GPIO_PORTF_IEV_R 		|= BUTTONS;    																// enable rising edge detection for PF4 & PF0
  GPIO_PORTF_ICR_R 	 	 = BUTTONS;      															// clear interrupt flags for PF4 & PF0
  GPIO_PORTF_IM_R 		|= BUTTONS;      															// arm interrupt on PF4
		
  NVIC_PRI7_R 			 	 = (NVIC_PRI7_R&0xFF1FFFFF)|0x00C00000; 			// priority 6
  NVIC_EN0_R 					|= NVIC_EN0_PORTF;      											// enable interrupt 30 in NVIC
	#endif
}


