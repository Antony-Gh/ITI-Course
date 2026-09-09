/*
 * MUSART_program.c
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/REGISTERS.h"
#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/USART/MUSART_config.h"
#include "../../HW/USART/MUSART_private.h"
#include "MUSART_interface.h"

void USART_voidInit(void) {
	u16 local_u16Ubrr = USART_PRIVATE_UBRR;

	/* URSEL must be 0 when writing UBRRH */
	UBRRH = (u8)(local_u16Ubrr >> 8U);
	UBRRL = (u8)local_u16Ubrr;

#if (USART_DOUBLE_SPEED == 1U)
	SET_BIT(UCSRA, U2X);
#else
	CLR_BIT(UCSRA, U2X);
#endif

	UCSRC = USART_PRIVATE_UCSRC_8N1;
	SET_BIT(UCSRB, RXEN);
	SET_BIT(UCSRB, TXEN);
}

USART_ErrorStatus_t USART_enumTransmit(u8 Copy_u8Data) {
	while (GET_BIT(UCSRA, UDRE) == 0U) {
		/* Wait until the transmit buffer is empty */
	}
	UDR = Copy_u8Data;
	return USART_OK;
}

USART_ErrorStatus_t USART_enumReceive(u8 *Copy_pu8Data) {
	USART_ErrorStatus_t Local_enumErrorState = USART_OK;

	if (Copy_pu8Data == NULL) {
		Local_enumErrorState = USART_ERR_NULL_POINTER;
	} else {
		while (GET_BIT(UCSRA, RXC) == 0U) {
			/* Wait until a byte is received */
		}
		*Copy_pu8Data = UDR;
	}

	return Local_enumErrorState;
}

u8 USART_u8IsRxReady(void) {
	return GET_BIT(UCSRA, RXC);
}

USART_ErrorStatus_t USART_enumTransmitString(const u8 *Copy_pu8String) {
	USART_ErrorStatus_t Local_enumErrorState = USART_OK;
	u16 local_u16Idx = 0U;

	if (Copy_pu8String == NULL) {
		Local_enumErrorState = USART_ERR_NULL_POINTER;
	} else {
		while (Copy_pu8String[local_u16Idx] != '\0') {
			USART_enumTransmit(Copy_pu8String[local_u16Idx]);
			local_u16Idx++;
		}
	}

	return Local_enumErrorState;
}
