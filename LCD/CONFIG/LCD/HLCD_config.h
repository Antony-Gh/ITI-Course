/*
 * HLCD_config.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#ifndef HLCD_CONFIG_H_
#define HLCD_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/* LCD Control Pins Configuration */
#define HLCD_CTRL_PORT      DIO_PORTB
#define HLCD_RS_PIN         DIO_PIN0
#define HLCD_RW_PIN         DIO_PIN1
#define HLCD_EN_PIN         DIO_PIN2

/* LCD Data Port Configuration */
#define HLCD_DATA_PORT      DIO_PORTA

#endif /* HLCD_CONFIG_H_ */
