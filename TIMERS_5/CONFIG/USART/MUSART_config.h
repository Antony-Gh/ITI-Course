/*
 * MUSART_config.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 */

#ifndef MUSART_CONFIG_H_
#define MUSART_CONFIG_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/*
 * Baud rate used with the PC / Virtual Terminal.
 * UBRR = F_CPU / (16 * baud) - 1  (normal speed, U2X = 0)
 * 8 MHz / 9600 -> UBRR = 51
 */
#define USART_BAUD_RATE 9600UL

/*
 * Double USART speed (U2X)
 *   0U : disabled (16x oversampling)
 *   1U : enabled  (8x oversampling)
 */
#define USART_DOUBLE_SPEED 0U

#endif /* MUSART_CONFIG_H_ */
