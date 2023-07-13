/*
 * a328_rn487x_com.c
 *
 * Created: 18.06.2023 19:09:22
 *  Author: Hans Philipp Zimmermann
 */ 

#define UBRR_VALUE	((F_CPU/(8*BAUD))-1)

#define TM_NONE			0
#define TM_PENDING		1
#define TM_COMPLETE		2
#define TM_NOTI			3


#include "a328_rn487x_com.h"

#include <inttypes.h>
#include <stddef.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile char RXbuffer[MAX_RX_BUFFER];

volatile uint8_t transmission_status;
volatile uint8_t bufferCounter;
volatile uint8_t timeOut_tick;

void (*dataRCPHandler)(const char* data);
void OnRxInterrupt();

/*
	Must be called regularly in a predefined time interval to provide a clock source for the timeout function of the transmission controller.
	When the function is called TIMEOUT_TICKS times, the timeout is raised
*/
void onTransmissionTimerTick();

ISR(USART_RX_vect)
{
	OnRxInterrupt();
}

ISR(TIMER2_OVF_vect)
{
	onTransmissionTimerTick();
}

void USART_Init()
{
	// set baud rate:
	UBRR0H = (unsigned char)(UBRR_VALUE >> 8);
	UBRR0L = (unsigned char)UBRR_VALUE;

	// double the transmission speed
	UCSR0A |= (1<<U2X0);
	
	// enable receiver and transmitter
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1<<RXCIE0);// enable receiver interrupt

	//set frame format
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);	// removed: (1 << URSEL)

	// enable global interrupt flag
	sei();
	
	// activate timer 2 for timeout condition tick source
	TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22); // prescaler = 1024 -> 9830400Hz / 1024 -> 9600 / 256 -> 37.5Hz ->  1 / 37.5 = 26.6ms
	TIMSK2 = (1<<TOIE2);
}

void USART_Write(signed char tx)
{
	// write a char into transmit buffer
	while (!(UCSR0A&(1 << UDRE0)));
	UDR0 = tx;
}

void USART_WriteString(const char* string)
{
	uint16_t i = 0;
	
	while((string[i] != '\0')&&(i < MAX_RX_BUFFER))
	{
		USART_Write(string[i]);
		i++;
	}
	_delay_ms(10);
}

void sendBLEData(const char* data)
{
	if(data != NULL)
	{
		USART_WriteString(data);
	}
}

void setDataReceptionHandler(void (*dataReceptionHandler)(const char* data))
{
	dataRCPHandler = dataReceptionHandler;
}

void ClearRxBuffer()
{
	for(uint8_t i = 0; i < bufferCounter; i++)
	{
		RXbuffer[i] = 0;
	}
	transmission_status = TM_NONE;
	bufferCounter = 0;
	timeOut_tick = 0;
}

void comInit(){
	USART_Init();
	bufferCounter = 255;
	dataRCPHandler = NULL;
	ClearRxBuffer();
	// wait for rn4870 uart (refer to data-sheet section 9 -Timing characteristics- FIGURE 9-2)
	_delay_ms(70);
}

/*
	Must be called regularly in a predefined time interval to provide a clock source for the timeout function of the transmission controller.
	When the function is called TIMEOUT_TICKS times, the timeout is raised
*/
void onTransmissionTimerTick(){
	if(transmission_status == TM_PENDING){
		timeOut_tick++;
	}
}

void checkTimeOutCondition(){
	if(timeOut_tick >= TIMEOUT_TICKS){
		ClearRxBuffer();
	}
}

void OnRxInterrupt()
{
	char rx = UDR0;
	
	timeOut_tick = 0;

	if((transmission_status != TM_NOTI) && (transmission_status != TM_COMPLETE))
	{
		RXbuffer[bufferCounter] = rx;
		bufferCounter++;
	}
	
	if((rx == '\0')||(rx == '\r')||(bufferCounter == MAX_RX_BUFFER))
	{
		// mark the transmission as complete and override the custom delimiter with a string terminator
		RXbuffer[bufferCounter - 1] = '\0';
		
		transmission_status = TM_COMPLETE;
	}
	else if(rx == 0x40)// notification pre + post delimiter (@ must be pre-configured as the delimiter)
	{
		// must be the start or the end of a rn4870 device notification
		if(transmission_status == TM_NOTI)
		{
			// end
			transmission_status = TM_NONE;
			RXbuffer[0] = 0;
			bufferCounter = 0;
		}
		else
		{
			// start
			transmission_status = TM_NOTI;
		}
	}
	else
	{
		if((transmission_status != TM_NOTI)&&(transmission_status != TM_COMPLETE))// prevent an override of the noti and complete - status
		{
			transmission_status = TM_PENDING;
		}
	}
}

void CopyTransmissionBuffer(volatile char* source, char* receiver)
{
	for(uint8_t i = 0; i < MAX_RX_BUFFER; i++)
	{
		receiver[i] = source[i];
	}
}

void fetchBleData()
{
	if(transmission_status == TM_COMPLETE)
	{
		char analyzeBuffer[MAX_RX_BUFFER];

		// copy the buffer
		CopyTransmissionBuffer(RXbuffer, analyzeBuffer);
		
		// erase the input-buffer and control parameter
		ClearRxBuffer();
		
		// perform reception handling
		if(dataRCPHandler != NULL)
		{
			dataRCPHandler(analyzeBuffer);
		}
	}
	else
	{
		if(transmission_status == TM_PENDING)
		{
			// schedule a timeout for the pending status to prevent a blocking queue
			// when timeout occurs: how to recognize if this is the same pending transmission or a new one ????
			checkTimeOutCondition();
		}
	}
}

