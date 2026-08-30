/*
 * HDCMOTOR_config.h
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#ifndef HDCMOTOR_CONFIG_H_
#define HDCMOTOR_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

#define HDCMOTOR_RELAY_PORT DIO_PORTD
#define HDCMOTOR_RELAY_CW_PIN DIO_PIN0  /* PD0 -> Q1 -> Relay K1 */
#define HDCMOTOR_RELAY_CCW_PIN DIO_PIN1 /* PD1 -> Q2 -> Relay K2 */

#define HDCMOTOR_RELAY_ACTIVE_LEVEL DIO_HIGH
#define HDCMOTOR_RELAY_INACTIVE_LEVEL DIO_LOW

/* Delay between relay de-energize and next energize */
#define HDCMOTOR_RELAY_SWITCH_DELAY_MS 50U

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#endif /* HDCMOTOR_CONFIG_H_ */
