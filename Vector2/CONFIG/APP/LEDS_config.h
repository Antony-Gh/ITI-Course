/*
 * LEDS_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef LEDS_CONFIG_H_
#define LEDS_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* 8 LEDs on Port A */
#define LEDS_PORT DIO_PORTA
#define LEDS_STEP_MS 150U                /* Delay between leds */
#define LEDS_ANIMATION_DURATION_MS 1500U /* The whole animation duration */

#endif /* LEDS_CONFIG_H_ */
