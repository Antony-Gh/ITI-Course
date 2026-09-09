/*
 * MEEPROM_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../LIB/REGISTERS.h"

#include "../../CONFIG/EEPROM/MEEPROM_config.h"
#include "../../HW/EEPROM/MEEPROM_private.h"

#include "MEEPROM_interface.h"

void MEEPROM_voidInit(void) {
	CLR_BIT(EECR, MEEPROM_EECR_EERIE);
}

u8 MEEPROM_u8IsBusy(void) {
	if (GET_BIT(EECR, MEEPROM_EECR_EEWE) == 1U) {
		return 1U;
	}

	return 0U;
}

u8 MEEPROM_u8WriteByte(u16 Copy_u16Address, u8 Copy_u8Data) {
	u32 Local_u32Timeout = 0UL;

	if (Copy_u16Address >= MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	while (GET_BIT(EECR, MEEPROM_EECR_EEWE) == 1U) {
		Local_u32Timeout++;

		if (Local_u32Timeout >= MEEPROM_TIMEOUT_VALUE) {
			return MEEPROM_TIMEOUT;
		}
	}

	EEAR = Copy_u16Address;

	EEDR = Copy_u8Data;

	SET_BIT(EECR, MEEPROM_EECR_EEMWE);

	SET_BIT(EECR, MEEPROM_EECR_EEWE);

	return MEEPROM_OK;
}

u8 MEEPROM_u8ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data) {
	u32 Local_u32Timeout = 0UL;

	if (Copy_pu8Data == NULL) {
		return MEEPROM_NULL_POINTER;
	}

	if (Copy_u16Address >= MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	while (GET_BIT(EECR, MEEPROM_EECR_EEWE) == 1U) {
		Local_u32Timeout++;

		if (Local_u32Timeout >= MEEPROM_TIMEOUT_VALUE) {
			return MEEPROM_TIMEOUT;
		}
	}

	EEAR = Copy_u16Address;

	SET_BIT(EECR, MEEPROM_EECR_EERE);

	*Copy_pu8Data = EEDR;

	return MEEPROM_OK;
}

u8 MEEPROM_u8WriteBlock(u16 Copy_u16Address, u8 *Copy_pu8Data, u16 Copy_u16Size) {
	u16 Local_u16Counter;
	u8 Local_u8Error;

	if (Copy_pu8Data == NULL) {
		return MEEPROM_NULL_POINTER;
	}

	if (Copy_u16Address >= MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	if ((Copy_u16Address + Copy_u16Size) > MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	for (Local_u16Counter = 0U; Local_u16Counter < Copy_u16Size;
			Local_u16Counter++) {
		Local_u8Error = MEEPROM_u8WriteByte(Copy_u16Address + Local_u16Counter,
				Copy_pu8Data[Local_u16Counter]);

		if (Local_u8Error != MEEPROM_OK) {
			return Local_u8Error;
		}

		while (MEEPROM_u8IsBusy())
			;
	}

	return MEEPROM_OK;
}

u8 MEEPROM_u8ReadBlock(u16 Copy_u16Address, u8 *Copy_pu8Data, u16 Copy_u16Size) {
	u16 Local_u16Counter;
	u8 Local_u8Error;

	if (Copy_pu8Data == NULL) {
		return MEEPROM_NULL_POINTER;
	}

	if (Copy_u16Address >= MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	if ((Copy_u16Address + Copy_u16Size) > MEEPROM_EEPROM_SIZE) {
		return MEEPROM_INVALID_ADDRESS;
	}

	for (Local_u16Counter = 0U; Local_u16Counter < Copy_u16Size;
			Local_u16Counter++) {
		Local_u8Error = MEEPROM_u8ReadByte(Copy_u16Address + Local_u16Counter,
				&Copy_pu8Data[Local_u16Counter]);

		if (Local_u8Error != MEEPROM_OK) {
			return Local_u8Error;
		}
	}

	return MEEPROM_OK;
}
