/*
 * APP_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/*
 * Buttons use internal pull-up: wire each button between the pin and GND.
 * Set APP_USE_LCD to 1 only if Port B is free for the LCD data bus.
 */
#define APP_USE_LCD 0
#define APP_USE_LED 1

#define APP_OFF_BUTTON_PORT DIO_PORTB
#define APP_OFF_BUTTON_PIN DIO_PIN6

#define APP_MODE_BUTTON_PORT DIO_PORTB
#define APP_MODE_BUTTON_PIN DIO_PIN7

#define APP_LED_POWER_IDX 0U
#define APP_LED_DIRECTION_IDX 1U

#endif /* APP_CONFIG_H_ */
