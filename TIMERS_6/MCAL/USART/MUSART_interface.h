/*
 * MUSART_interface.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 */

#ifndef MUSART_INTERFACE_H_
#define MUSART_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

typedef enum {
	USART_OK = 0,
	USART_ERR_NULL_POINTER
} USART_ErrorStatus_t;

void USART_voidInit(void);
USART_ErrorStatus_t USART_enumTransmit(u8 Copy_u8Data);
USART_ErrorStatus_t USART_enumReceive(u8 *Copy_pu8Data);
u8 USART_u8IsRxReady(void);
USART_ErrorStatus_t USART_enumTransmitString(const u8 *Copy_pu8String);

#endif /* MUSART_INTERFACE_H_ */
