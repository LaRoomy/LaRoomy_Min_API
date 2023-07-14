#ifndef _TIMEKEEPER_H_
#define _TIMEKEEPER_H_

#include <util/twi.h>

#include "atmega328_twi.h"
#include "BitUtilitiesEx.h"

// For complete documentation take a look at the data sheet of Microchip MCP 79410
// URL: http://ww1.microchip.com/downloads/en/devicedoc/20002266h.pdf

/*
	NOTES:
		- before the device is working, the oscillator must be started, don't forget!!
		- to enable the oscillator, the bit 7 in the seconds register must be set! (ST_OSCILLATOR_ENABLE)
		- the PWRFAIL flag does not indicate that the backup-battery is empty, it indicates only that the primary power was lost
		- if a full power loss occurred, the oscillator is stopped on startup, check this
		- if the data is set with 'setDate()' function, the backup battery will be enabled or disabled dependent to the MCP79410_USE_BACKUP_BATTERY definition, respectively
		- if the 'setDate()' function is not used, the backup battery must be activated explicitly by the 'enableBackupBattery()' function
*/

#define		MCP79410_RTCC_SRAM_SLAVE_ADDRESS			0xDE
//#define		MCP79410_RTCC_SRAM_SLAVE_ADDRESS_READ		0xDF
//#define		MCP79410_RTCC_SRAM_SLAVE_ADDRESS_WRITE		0xDE

#define		MCP79410_EEPROM_SLAVE_ADDRESS				0xAE
//#define		MCP79410_EEPROM_SLAVE_ADDRESS_READ			0xAF
//#define		MCP79410_EEPROM_SLAVE_ADDRESS_WRITE			0xAE


// MCP79410 TIMEKEEPER REGISTER-ADDRESSES:
//___________________________________________________________________________________________________________________________________________________________
// Timekeeping (0x0 .. 0x09)
#define	TCREG_SECONDS			0x00	// Bit 0~6 Seconds / Bit 7: Oscillator Enable!
#define TCREG_MINUTES			0x01	// Bit 0~6 Minutes / Bit 7 not used
#define	TCREG_HOURS				0x02	// Bit 6 12-or-24h / Bit 5 AmPm-or-10h / Bit 4 10Hours / Bit 0~3 Hours / Bit 7 not used (if 24 hours is set, bit 4:5 is the second digit hour value: 1 or 2)
#define	TCREG_WKDAY_V_P_O		0x03	// Bit 0~2 Day( Monday (1) - Sunday (7)) / Bit 3 VBat Enable / Bit 4 Power fail / Bit 5 Oscillator run status
#define	TCREG_DATE				0x04	// Bit 0~3 Date / Bit 4~5 10_Date (0-3)
#define	TCREG_MONTH				0x05	// Bit 0~4 Month / Bit 5 Leap-Year bit / Bit 6~7 not used
#define	TCREG_YEAR				0x06	// Bit 0~7 Year
#define	TC_CONTROLREGISTER		0x07	// Control Register
#define TC_OSC_TRIMREGISTER		0x08	// Oscillator trim Register
//#define TC_EEPROM_UNLOCK_REG	0x09	// EERPOM Unlock Register (Protected)
//______________________________________________________________________________________
// Alarm-0 (0x0A .. 0x0F)
#define	TCREG_ALARM0_SEC		0x0A	// Bit 0~6 Alarm 1 seconds / Bit 7 not used
#define	TCREG_ALARM0_MIN		0x0B	// Bit 0~6 Alarm 1 minutes / Bit 7 not used
#define	TCREG_ALARM0_HOUR		0x0C	// like register 0x02 / Bit 7 not used
#define	TCREG_ALARM0_WKDAY		0x0D	// Bit 0~2 Day (1 .. 7) /  Bit 3 Alarm interrupt flag / Bit 4~6 Alarm mask bits / Bit 7 Alarm interrupt output polarity 
#define TCREG_ALARM0_DATE		0x0E	// Bit 0~3 Date first digit (0 .. 9) / Bit 4~5 Date second digit (0 .. 3) / Bit 6~7 not used
#define TCREG_ALARM0_MONTH		0x0F	// Bit 0~3 Month first digit (0 .. 9) / Bit 4 Month second digit (tenth digit)
// 0x10 - Reserved - do not use 0x10	// Do not use
//______________________________________________________________________________________
// Alarm-1 (0x11 .. 0x16)
#define	TCREG_ALARM1_SEC		0x11	// Bit 0~6 Alarm 1 seconds / Bit 7 not used
#define	TCREG_ALARM1_MIN		0x12	// Bit 0~6 Alarm 1 minutes / Bit 7 not used
#define	TCREG_ALARM1_HOUR		0x13	// like register 0x02 / Bit 7 not used
#define	TCREG_ALARM1_WKDAY		0x14	// Bit 0~2 Day (1 .. 7) /  Bit 3 Alarm interrupt flag / Bit 4~6 Alarm mask bits / Bit 7 Alarm interrupt output polarity
#define TCREG_ALARM1_DATE		0x15	// Bit 0~3 Date first digit (0 .. 9) / Bit 4~5 Date second digit (0 .. 3) / Bit 6~7 not used
#define TCREG_ALARM1_MONTH		0x16	// Bit 0~3 Month first digit (0 .. 9) / Bit 4 Month second digit (tenth digit)
// 0x17 - Reserved - do not use 0x17	// Do not use
//______________________________________________________________________________________
// Power-Down Timestamp (0x18 .. 0x1B)
#define TCREG_POWERDOWN_MIN		0x18	// Bit 0~6 Power-down minutes
#define TCREG_POWERDOWN_HOURS	0x19	// Bit 0~3 Hour one digit / Bit 4~5 Hour ten digit / Bit 6 12/24 Hours / Bit 7 not used
#define TCREG_POWERDOWN_DATE	0x1A	// Bit 0~3 Date one digit / Bit 4~5 Date ten digit / Bit 6~7 not used
#define TCREG_POWERDOWN_MONTH	0x1B	// Bit 0~3 Month ten digit / Bit 4 Month ten digit / Bit 5~7 Day of Week digit (0 .. 7)
//______________________________________________________________________________________
// Power-Up Timestamp (0x1C .. 0x1F)
#define TCREG_POWERUP_MIN		0x1C	// Bit 0~6 Power-up minutes
#define TCREG_POWERUP_HOURS		0x1D	// Bit 0~3 Hour one digit / Bit 4~5 Hour ten digit / Bit 6 12/24 Hours / Bit 7 not used
#define TCREG_POWERUP_DATE		0x1E	// Bit 0~3 Date one digit / Bit 4~5 Date ten digit / Bit 6~7 not used
#define TCREG_POWERUP_MONTH		0x1F	// Bit 0~3 Month ten digit / Bit 4 Month ten digit / Bit 5~7 Day of Week digit (0 .. 7)


// TIMEKEEPER REGISTER FLAGS
//__________________________________________________________
// Control Register:			address: 0x07
#define		SQWFS0		0x01	// Square Wave Clock Output Frequency Select bit 1
#define		SQWFS1		0x02	// Square Wave Clock Output Frequency Select bit 2
#define		CRSTRIM		0x04	// Coarse Trim Mode Enable bit
#define		EXTOSC		0x08	// External Oscillator input bit (1 == Enable X1 pin to be driven by external 32.768 kHz source / 0 == disable)
#define		ALM0EN		0x10	// Alarm 0 enable control bit (1 == enabled / 0 == disabled)
#define		ALM1EN		0x20	// Alarm 1 enable control bit (1 == enabled / 0 == disabled)
#define		SQWEN		0x40	// Square-wave enable control bit (1 == enabled / 0 == disabled)
#define		OUT			0x80	// Logic level for GPIO pin (1 == enabled / 0 == disabled) / (ignored if square-wave or alarm is set)

// Other control Flags
#define		ST_OSCILLATOR_ENABLE	0x80 // included in the seconds register 0x00

// Status FLAGS:
#define		OSCRUN_STATUS		0x20	// included in the week-day register 0x03
#define		PWRFAIL_STATUS		0x10	// included in the week-day register 0x03	// indicates power fail, must be cleared by software, clearing this bit resets the power-fail timestamp registers to 0
#define		VBATENABLE_CTRL		0x08	// included in the week-day register 0x03	// enables the backup battery input pin

// Trim Register:				address: 0x08
//#define		TRIMVAL 0~6			0x01 ~ 0x40
#define		SIGN				0x80	// direction for applying the trim value (1 == add / 0 == subtract)

// Alarm Register Control and Status:
#define		ALMxIF_STATUS		0x08	// Alarm interrupt Flag bit (1 == Alarm match occurred (must be cleared in software))
// #define ALMxMSK 4~6	0x10 ~ 0x40		// Alarm mask bits
#define		ALMPOS_CTRL			0x80	// Alarm Interrupt Output Polarity bit (1 == Asserted output state of MFP is a logic high level / 0 == low level)
//___________________________________________________________________________________________________________________________________________________________

// to use the Vbackup-pin function (Backup Battery) set this to '1'
#define		MCP79410_USE_BACKUP_BATTERY		1

uint8_t BCD_TO_BYTE(uint8_t BCDvalue)
{
	return ((BCDvalue/16*10)+(BCDvalue%16));
}

uint8_t BYTE_TO_BCD(uint8_t BYTEvalue)
{
	return ((BYTEvalue/10*16)+(BYTEvalue%10));
}

uint8_t SetTimekeeperControlRegister(uint8_t ctrlRegister)
{
	uint8_t result;

	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(TC_CONTROLREGISTER);
			if (result)
			{
				result = TWI_SendData(ctrlRegister);
			}
		}
		TWI_Stop();
	}
	return result;
}

uint8_t SetTimeKeeperRTCCRegister(uint8_t registerAddress, uint8_t value)
{
	uint8_t result;

	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(registerAddress);
			if (result)
			{
				result = TWI_SendData(value);
			}
		}
		TWI_Stop();
	}
	return result;
}

uint8_t GetTimekeeperRTCCRegister(uint8_t registerAddress)
{
	uint8_t result;
	uint8_t data = 0;

	TWI_Reset();
	// first set register-pointer
	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(registerAddress);

			// then read register
			if (result)
			{
				result = TWI_Start();
				if (result)
				{
					result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_READ);
					if (result)
					{
						data = TWI_ReceiveData();
					}
				}
			}
		}
		TWI_Stop();
	}
	return data;
}

uint8_t Timekeeper_CheckPrimaryPowerLoss()// return value: nonzero if the timekeeper lost primary power, zero otherwise
{
	uint8_t reg = GetTimekeeperRTCCRegister(TCREG_WKDAY_V_P_O); // this register contains the power-fail-flag
	if (reg & PWRFAIL_STATUS)
		return 1;
	else
		return 0;
}

uint8_t Timekeeper_CheckFullPowerLoss()// return value: nonzero if the power was lost, zero otherwise
{
	uint8_t reg = GetTimekeeperRTCCRegister(TCREG_WKDAY_V_P_O); // check if the oscillator is running, if not, the power must be lost
	if (reg & OSCRUN_STATUS){
		return 0;
	}
	else {
		return 1;
	}
}



uint8_t Timekeeper_SetTime(uint8_t hr, uint8_t min, uint8_t sec)// set the current time to the rtc, here we use the 24-hour format!
{
	uint8_t result;

	TWI_Reset();

	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(TCREG_SECONDS);
			if (result)
			{
				// bit 7 of the seconds-register contains also the oscillator enable flag, so make sure to write a '1' to it, otherwise the oscillator will stop
				uint8_t sec_register = BYTE_TO_BCD(sec);
				sec_register |= ST_OSCILLATOR_ENABLE;

				result = TWI_SendData(sec_register);// send seconds
				if (result)
				{
					result = TWI_SendData(BYTE_TO_BCD(min));// send minutes
					if (result)
					{
						// make sure to set the right format for the hours (12/24)
						uint8_t hour_register = BYTE_TO_BCD(hr);
						//hour_register |= (1<<0x40)					// set 12 hour format // not used here!

						result = TWI_SendData(hour_register);// send hours
					}
				}
			}
		}
		TWI_Stop();
	}
	return result;
}

uint8_t Timekeeper_SetDate(uint8_t date, uint8_t dayofWeek, uint8_t month, uint8_t year)
{
	uint8_t result;

	TWI_Reset();

	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(TCREG_WKDAY_V_P_O);
			if (result)
			{
				// the week-day register also contains status and control flags, so make sure to write no bullshit to it!

				uint8_t dow = BYTE_TO_BCD(dayofWeek);
				
				if(MCP79410_USE_BACKUP_BATTERY)
				{
					dow |= VBATENABLE_CTRL;
				}
				
				result = TWI_SendData(dow);// send dayofWeek
				if (result)
				{
					result = TWI_SendData(BYTE_TO_BCD(date));// send dayofMonth (date)
					if (result)
					{
						// the month-register also contains a bit for the leap year, we ignore it here
						result = TWI_SendData(BYTE_TO_BCD(month));// send month
						if (result)
						{
							result = TWI_SendData(BYTE_TO_BCD(year));// send year
						}
					}
				}
			}
		}
		TWI_Stop();
	}
	return result;
}

uint16_t GetSystemTime()
{
	uint8_t hr = GetTimekeeperRTCCRegister(TCREG_HOURS);// if 24-hour format is used, there is no need for further conversations
	uint8_t min = GetTimekeeperRTCCRegister(TCREG_MINUTES);

	return MAKEWORD(
		BCD_TO_BYTE(hr),
		BCD_TO_BYTE(min)	// low-word: hours / high-word: minutes
	);
}

void GetSystemDate(uint8_t* date, uint8_t* dayOfWeek, uint8_t* month, uint8_t* year)
{
	uint8_t tcreg = GetTimekeeperRTCCRegister(TCREG_DATE);
	*date = BCD_TO_BYTE(tcreg);
	tcreg = GetTimekeeperRTCCRegister(TCREG_WKDAY_V_P_O);
	*dayOfWeek = BCD_TO_BYTE(tcreg & 0x07);
	tcreg = GetTimekeeperRTCCRegister(TCREG_MONTH);
	*month = BCD_TO_BYTE(tcreg & 0x1f);
	tcreg = GetTimekeeperRTCCRegister(TCREG_YEAR);
	*year = BCD_TO_BYTE(tcreg & 0x0f);
	*year += (10*BCD_TO_BYTE(((tcreg & 0xf0)>>4)));
}

uint8_t Timekeeper_SetAlarm0(/*TODO*/)
{
	uint8_t result;

	result = TWI_Start();
	if (result)
	{
		result = TWI_SendTargetAddressAndMode(MCP79410_RTCC_SRAM_SLAVE_ADDRESS, TWIMD_WRITE);
		if (result)
		{
			result = TWI_SetDataRegisterPointer(TCREG_ALARM0_SEC);
			if (result)
			{
				// TODO: transmit all necessary values
			}
		}
		TWI_Stop();
	}
	return result;
}

uint8_t Timekeeper_SetAlarm1()
{
	// not implemented
	return 0;
}

/*
	Must be reset every time the oscillator was stopped
*/
void Timekeeper_EnableBackupBattery()
{
	uint8_t regValue;
	
	regValue = GetTimekeeperRTCCRegister(TCREG_WKDAY_V_P_O);
	SetTimeKeeperRTCCRegister(TCREG_WKDAY_V_P_O, (regValue | VBATENABLE_CTRL));
}

void Timekeeper_ErasePowerFailFlag()
{
	uint8_t regValue =
		GetTimekeeperRTCCRegister(TCREG_WKDAY_V_P_O);
		
	regValue &= ~(PWRFAIL_STATUS);
	
	SetTimeKeeperRTCCRegister(TCREG_WKDAY_V_P_O, regValue);
}

void Timekeeper_EnableOscillator(uint8_t enable){
	
	uint8_t regValue =
		GetTimekeeperRTCCRegister(TCREG_SECONDS);
	
	if(enable){
		regValue |= ST_OSCILLATOR_ENABLE;
	}
	else {
		regValue &= ~(ST_OSCILLATOR_ENABLE);
	}
	SetTimeKeeperRTCCRegister(TCREG_SECONDS, regValue);
}

uint8_t Timekeeper_SetTrimRegister(uint8_t trimValue){
	return SetTimeKeeperRTCCRegister(TC_OSC_TRIMREGISTER, trimValue);
}


#endif //_TIMEKEEPER_H_


