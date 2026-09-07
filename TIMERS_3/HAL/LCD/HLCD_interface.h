/*
 * HLCD_interface.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#ifndef HLCD_INTERFACE_H_
#define HLCD_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

void HLCD_voidInit(void);

void HLCD_voidSendCommand(u8 Copy_u8Command);

void HLCD_voidSendData(u8 Copy_u8Data);

void HLCD_voidSendString(const char* Copy_pcString);

void HLCD_voidGoToXY(u8 Copy_u8Row, u8 Copy_u8Col);

void HLCD_voidClearScreen(void);

void HLCD_voidSendStringTypingEffect(const char* Copy_pcString, u32 Copy_u32DelayMs);

void HLCD_voidCreateCustomChar(const u8* Copy_pu8Pattern, u8 Copy_u8Location);

#endif /* HLCD_INTERFACE_H_ */
