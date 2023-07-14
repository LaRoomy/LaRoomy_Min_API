/*
 * atmega328_twi.h
 *
 * Created: 01.09.2021 11:33:30
 * Author: Hans Philipp Zimmermann
 *
 *
 */ 


#ifndef ATMEGA328_TWI_H_
#define ATMEGA328_TWI_H_

#include <util/twi.h>

// TWI STATUS FLAGS
#define	STARTCOND_TRANSMIT_SUCCESS				0x08
#define	REPEATED_STARTCOND_TRANSMIT_SUCCESS		0x10
#define	WRITEMODE_ADDRESS_TRANSMIT_SUCCESS		0x18
#define DATA_TRANSMIT_SUCCESS					0x28

#define	READMODE_ADDRESS_TRANSMIT_SUCCESS		0x40
#define	DATA_RECEIVED_SUCCESS					0x50


#define	TWIMD_READ		0x01
#define TWIMD_WRITE		0


void TWINT_Wait()
{
	while (!(TWCR & (1 << TWINT)));
}

uint8_t Check_Status(uint8_t statusToCheck)
{
	if ((TWSR & 0xf8) != statusToCheck)
		return 0;
	else
		return 1;
}

uint8_t TWI_Start()
{
	TWBR = 0x06;
	TWSR |= (1<<TWPS0);
	TWCR |= (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

	TWINT_Wait();

	if (Check_Status(STARTCOND_TRANSMIT_SUCCESS)
		|| Check_Status(REPEATED_STARTCOND_TRANSMIT_SUCCESS))
		return 1;
	else
		return 0;
}

void TWI_Stop()
{
	TWCR |= (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_Reset()
{
	TWCR &= ~((1 << TWSTO) | (1 << TWEN));
	TWCR |= (1 << TWEN);
}

uint8_t TWI_SendTargetAddressAndMode(uint8_t address, uint8_t mode)// set device address and mode (read or write)
{
	TWDR = address;
	TWDR |= mode;
	
	TWCR |= (1 << TWINT) | (1 << TWEN);

	TWINT_Wait();

	if (Check_Status(WRITEMODE_ADDRESS_TRANSMIT_SUCCESS)
		|| Check_Status(READMODE_ADDRESS_TRANSMIT_SUCCESS))
		return 1;
	else
		return 0;
}


uint8_t TWI_SetDataRegisterPointer(uint8_t registerAddress)
{
	TWDR = registerAddress;
	TWCR = (1 << TWINT) | (1 << TWEN);

	TWINT_Wait();
	return Check_Status(DATA_TRANSMIT_SUCCESS);
}

uint8_t TWI_SendData(uint8_t data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);

	TWINT_Wait();
	return Check_Status(DATA_TRANSMIT_SUCCESS);
	//return 1;
}

uint8_t TWI_ReceiveData()
{
	TWCR = (1 << TWINT) | (1 << TWEN);// read with NACK to conclude transmission after register is received
	TWINT_Wait();
	return TWDR;
}

#endif /* ATMEGA328_TWI_H_ */