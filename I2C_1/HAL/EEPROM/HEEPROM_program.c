/*
 * HEEPROM_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/EEPROM/HEEPROM_config.h"
#include "../../HW/EEPROM/HEEPROM_private.h"

#include "../../MCAL/I2C/MI2C_interface.h"

#include "HEEPROM_interface.h"

void HEEPROM_voidInit(void) {
	MI2C_voidInit();
}

u8 HEEPROM_u8WriteByte(u16 Copy_u16Address, u8 Copy_u8Data) {
	u8 Local_u8Error;

	Local_u8Error = MI2C_u8Start();

	if (Local_u8Error != MI2C_OK) {
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8SendAddress((HEEPROM_ADDRESS << 1) |
	HEEPROM_WRITE_ADDRESS);

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8WriteData((u8) Copy_u16Address);

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8WriteData(Copy_u8Data);

	MI2C_voidStop();

	if (Local_u8Error != MI2C_OK) {
		return HEEPROM_ERROR;
	}

	return HEEPROM_OK;
}

u8 HEEPROM_u8ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data) {
	u8 Local_u8Error;

	if (Copy_pu8Data == NULL) {
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8Start();

	if (Local_u8Error != MI2C_OK) {
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8SendAddress((HEEPROM_ADDRESS << 1) |
	HEEPROM_WRITE_ADDRESS);

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8WriteData((u8) Copy_u16Address);

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8Start();

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8SendAddress((HEEPROM_ADDRESS << 1) |
	HEEPROM_READ_ADDRESS);

	if (Local_u8Error != MI2C_OK) {
		MI2C_voidStop();
		return HEEPROM_ERROR;
	}

	Local_u8Error = MI2C_u8ReadDataWithNACK(Copy_pu8Data);

	MI2C_voidStop();

	if (Local_u8Error != MI2C_OK) {
		return HEEPROM_ERROR;
	}

	return HEEPROM_OK;
}
