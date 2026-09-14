/*
 * HLCD_I2C_interface.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Standalone I2C LCD driver using PCF8574
 */

#ifndef HLCD_I2C_INTERFACE_H_
#define HLCD_I2C_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

void HLCD_I2C_voidInit(void);
void HLCD_I2C_voidSendCommand(u8 Copy_u8Command);
void HLCD_I2C_voidSendData(u8 Copy_u8Data);
void HLCD_I2C_voidSendString(const char* Copy_pcString);
void HLCD_I2C_voidSendNumber(u32 Copy_u32Number);
void HLCD_I2C_voidGoToXY(u8 Copy_u8Row, u8 Copy_u8Col);
void HLCD_I2C_voidClearScreen(void);
void HLCD_I2C_voidSendStringTypingEffect(const char* Copy_pcString, u32 Copy_u32DelayMs);
void HLCD_I2C_voidCreateCustomChar(const u8* Copy_pu8Pattern, u8 Copy_u8Location);
void HLCD_I2C_voidSendFrequency(u32 Copy_u32FreqHz);

#endif /* HLCD_I2C_INTERFACE_H_ */
