/*
 * main.c
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 *
 * USART LED control:
 *   Receive an ASCII digit from the PC and toggle that LED on PORTA.
 *   '1' -> PA0, '2' -> PA1, ... '8' -> PA7
 */

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/LED/HLED_interface.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/USART/MUSART_interface.h"

void setup(void) {
	DIO_voidInit();
	HLED_voidInit();
	USART_voidInit();
	USART_enumTransmitString((const u8 *)"Send 1-8 to toggle LEDs\r\n");
}

void loop(void) {
	u8 local_u8Byte;
	u8 local_u8LedIdx;

	if (USART_u8IsRxReady() == 0U) {
		return;
	}

	if (USART_enumReceive(&local_u8Byte) != USART_OK) {
		return;
	}

	if ((local_u8Byte >= APP_USART_LED_MIN_CHAR) &&
	    (local_u8Byte <= APP_USART_LED_MAX_CHAR)) {
		local_u8LedIdx = (u8)(local_u8Byte - APP_USART_LED_MIN_CHAR);
		HLED_voidToggleLed(local_u8LedIdx);
		USART_enumTransmit(local_u8Byte);
	}
}

int main(void) {
	setup();

	while (1) {
		loop();
	}

	return 0;
}
