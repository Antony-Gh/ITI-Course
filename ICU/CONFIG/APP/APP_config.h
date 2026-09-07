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
 * Hardware:
 *   Timer0 CTC 1 ms tick drives PWM on PB3 (OC0 pin as GPIO)
 *   EXTI0 ICU input INT0 -> PD2
 * Connect a jumper from PB3 to PD2.
 *
 * 50 Hz -> 20 ms period
 * 45% duty -> 9 ms high, 11 ms low
 */
#define APP_PWM_PORT DIO_PORTB
#define APP_PWM_PIN DIO_PIN3
#define APP_ICU_PORT DIO_PORTD
#define APP_ICU_PIN DIO_PIN2

#define APP_PWM_FREQ_HZ 50U
#define APP_PWM_DUTY_PERCENT 45U
#define APP_PWM_PERIOD_MS 20U
#define APP_PWM_ON_MS 9U

/* LCD refresh period while a valid ICU frame is available */
#define APP_LCD_REFRESH_MS 250U

#endif /* APP_CONFIG_H_ */
