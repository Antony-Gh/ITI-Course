/*
 * HEEPROM_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef HAL_EEPROM_HEEPROM_INTERFACE_H_
#define HAL_EEPROM_HEEPROM_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define HEEPROM_OK              0U
#define HEEPROM_ERROR           1U

void HEEPROM_voidInit(void);

u8 HEEPROM_u8WriteByte(u16 Copy_u16Address, u8 Copy_u8Data);

u8 HEEPROM_u8ReadByte(u16 Copy_u16Address, u8 *Copy_pu8Data);

#endif /* HAL_EEPROM_HEEPROM_INTERFACE_H_ */
