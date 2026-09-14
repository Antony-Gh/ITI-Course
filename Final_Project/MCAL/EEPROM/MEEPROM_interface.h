/*
 * MEEPROM_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef MCAL_EEPROM_MEEPROM_INTERFACE_H_
#define MCAL_EEPROM_MEEPROM_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define MEEPROM_OK                  0U
#define MEEPROM_ERROR               1U
#define MEEPROM_INVALID_ADDRESS     2U
#define MEEPROM_TIMEOUT             3U
#define MEEPROM_NULL_POINTER        4U

void MEEPROM_voidInit(void);

u8 MEEPROM_u8WriteByte(u16 Copy_u16Address, u8 Copy_u8Data);

u8 MEEPROM_u8ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data);

u8 MEEPROM_u8WriteBlock(u16 Copy_u16Address, u8 *Copy_pu8Data, u16 Copy_u16Size);

u8 MEEPROM_u8ReadBlock(u16 Copy_u16Address, u8 *Copy_pu8Data, u16 Copy_u16Size);

u8 MEEPROM_u8IsBusy(void);

#endif /* MCAL_EEPROM_MEEPROM_INTERFACE_H_ */
