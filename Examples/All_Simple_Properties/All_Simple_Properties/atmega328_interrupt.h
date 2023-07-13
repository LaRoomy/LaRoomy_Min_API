/*
 * atmega328_interrupt.h
 *
 * Author: Hans Philipp Zimmermann
 */ 

#ifndef ATMEGA328_INTERRUPT_H_
#define ATMEGA328_INTERRUPT_H_

//	EICRA > External Interrupt Control Register A (ISC11 | ISC10 | ISC01 | ISC00) ?= selection for the INT0 and the INT1 Edge selection
//	EIMSK > External Interrupt Mask Register (INT0 | INT1) ?= activate the interrupt
//
//	http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf


#include <avr/interrupt.h>

void setupINT0_Interrupt(){
	// falling edge on INT0 generates an interrupt	
	EICRA = (1<<ISC01);	
	EIMSK |= (1<<INT0);
}

void enablePinChangeInterruptOnStatusPin()
{
	PCICR = (1<<PCIE2);
	PCMSK2 = (1<<PCINT22);//|(1<<PCINT23);
	sei();
}

#endif /* ATMEGA328_INTERRUPT_H_ */