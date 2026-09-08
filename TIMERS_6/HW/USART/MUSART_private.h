/*
 * MUSART_private.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 */

#ifndef MUSART_PRIVATE_H_
#define MUSART_PRIVATE_H_

#include "../../LIB/REGISTERS.h"
#include "../../CONFIG/USART/MUSART_config.h"

#if (USART_DOUBLE_SPEED == 1U)
#define USART_PRIVATE_UBRR \
	((u16)((F_CPU / (8UL * USART_BAUD_RATE)) - 1UL))
#else
#define USART_PRIVATE_UBRR \
	((u16)((F_CPU / (16UL * USART_BAUD_RATE)) - 1UL))
#endif

/* 8-bit data, no parity, 1 stop bit; URSEL selects UCSRC over UBRRH */
#define USART_PRIVATE_UCSRC_8N1 ((1U << URSEL) | (1U << UCSZ1) | (1U << UCSZ0))

#endif /* MUSART_PRIVATE_H_ */
