/*
 * APP_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/EXTI/MEXTI_interface.h"

/*
 * Toggle button on INT0 (PD2): wire button between PD2 and GND.
 * Internal pull-up is enabled in firmware.
 */
#define APP_USE_LCD 0
#define APP_USE_LED 1

#define APP_TOGGLE_BUTTON_PORT DIO_PORTD
#define APP_TOGGLE_BUTTON_PIN DIO_PIN2
#define APP_TOGGLE_EXTI_ID EXTI_u8_INT0

#define APP_LED_POWER_IDX 0U /* PC0: ON when motor is running */
#define APP_LED_OFF_IDX 1U   /* PC1: ON when motor is stopped */

#endif /* APP_CONFIG_H_ */
