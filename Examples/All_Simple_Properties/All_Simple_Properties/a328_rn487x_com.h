/*
 * a328_rn487x_com.h
 *
 *  Author: Hans Philipp Zimmermann
 */ 

#ifndef A328_RN487X_COM_H_
#define A328_RN487X_COM_H_

#include "proj_speed.h"

#define	TIMEOUT_TICKS	3	// transmission timeout duration (1 tick == 26.6ms)

#define	MAX_RX_BUFFER	255 // transmission size limited to 255

#ifndef F_CPU
#define F_CPU	9830400UL
# warning "F_CPU not defined for usart communication"
#endif // F_CPU

#ifndef BAUD
#define BAUD	57600UL
# warning "Baudrate not defined for usart communication"
#endif // BAUD

void comInit();
void setDataReceptionHandler(void (*dataReceptionHandler)(const char*));
void sendBLEData(const char* data);
void fetchBleData();

#endif /* A328_RN487X_COM_H_ */