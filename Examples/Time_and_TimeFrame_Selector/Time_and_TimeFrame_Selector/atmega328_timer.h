/*
 * atmega328_timer.h
 *
 *  Author: Hans Philipp Zimmermann
 */ 
#ifndef ATMEGA328_TIMER_H_
#define ATMEGA328_TIMER_H_

#define PWM_CHANNEL_A	1
#define PWM_CHANNEL_B	2

#include <avr/interrupt.h>

//#define PWM_TABLE_SIZE	42
//
//const uint8_t pwm_table[PWM_TABLE_SIZE] =
//{
	//2, 3, 4, 5, 6, 7, 8, 10, 11, 13, 16, 19, 23,
	//27, 32, 35, 38, 41, 45, 50, 54, 59, 64, 69, 76, 83, 91, 100,
	//108, 117, 128, 138, 152, 165, 181, 197, 208, 215, 222, 230, 238, 255
//};

volatile uint8_t timerCounter;
volatile uint8_t timerEvent;
//volatile uint8_t timerEvent_2sec;

void activateTimer0()
{
	timerCounter = 0;
	timerEvent = 0;
	//timerEvent_2sec = 0;
	
	TCCR0B = (1<<CS00) | (1<<CS02);		// prescaler = 1024 -> 9830400Hz / 1024 -> 9600 / 256 -> 37.5Hz -> 1 / 37.5 = 26.6ms
	TIMSK0 = (1<<TOIE0);
	
	sei();
}

void activateTimer1()
{
	TCCR1B = (1<<CS10);		// no prescaling =  9830400Hz / 65536 -> 150Hz -> 1 / 150 = 6.6ms
	TIMSK1 = (1<<TOIE1);
}

// NOTE: timer 2 is reserved for the timeout timer of the transmission controller

//void activateTimer2()
//{
	//TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22); // prescaler = 1024 -> 9830400Hz / 1024 -> 9600 / 256 -> 37.5Hz ->  1 / 37.5 = 26.6ms
	//TIMSK2 = (1<<TOIE2);
//}

void InitPWMTimer1()
{
	// Waveform-Generation-Mode: Fast PWM 8bit
	// Timer Prescaler clk/8
	TCCR1A |= (1<<WGM10);
	TCCR1B |= (1<<WGM12)|(1<<CS11);
}

void StopPWMTimer1()
{
	TCCR1A = 0;
	TCCR1B = 0;
}

void StartPWM(uint8_t channel)
{
	if(channel == PWM_CHANNEL_A)
	TCCR1A |= (1<<COM1A1);// clear channel A on compare match

	if(channel == PWM_CHANNEL_B)
	TCCR1A |= (1<<COM1B1);// clear channel B on compare match
}

void StopPWM(uint8_t channel)
{
	if(channel == PWM_CHANNEL_A)
	{
		TCCR1A &= ~(1<<COM1A1);// normal port operation on channel A
		PORTB &= ~(1<<PWM_CHANNEL_A);
	}

	if(channel == PWM_CHANNEL_B)
	{
		TCCR1A &= ~(1<<COM1B1);// normal port operation on channel B
		PORTB &= ~(1<<PWM_CHANNEL_B);
	}
}

void SetPWMValue(uint8_t channel, uint8_t value)
{
	if(channel == PWM_CHANNEL_A)
	OCR1A = value;
	else if(channel == PWM_CHANNEL_B)
	OCR1B = value;
}

// uint8_t uint8_to_pwmTableIndex(uint8_t val)
// {
// 	for(uint8_t i = 0; i < PWM_TABLE_SIZE; i++)
// 	{
// 		if(val <= pwm_table[i])
// 		{
// 			return i;
// 		}
// 	}
// 	return 0;
// }

#endif /* ATMEGA328_TIMER_H_ */