/*
 * KPD_I2C_interface.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Standalone I2C Keypad driver
 */

#ifndef KPD_I2C_INTERFACE_H_
#define KPD_I2C_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define KPD_I2C_NOT_PRESSED 0xFF

void KPD_I2C_voidInit(void);
u8 KPD_I2C_u8ScanKey(void);
u8 KPD_I2C_u8GetPressedKey(void);

#endif /* KPD_I2C_INTERFACE_H_ */
