/*
 * APP_config.h
 *
 *  Created on: Sep 7, 2026
 *      Author: Anthony Gaius
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

/*
 * PC sends ASCII '1'..'8' over USART.
 * '1' toggles LED 1 on PA0, '8' toggles LED 8 on PA7.
 */
#define APP_USART_LED_MIN_CHAR '1'
#define APP_USART_LED_MAX_CHAR '8'

#endif /* APP_CONFIG_H_ */
