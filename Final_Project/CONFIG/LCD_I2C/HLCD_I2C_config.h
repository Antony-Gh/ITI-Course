/*
 * HLCD_I2C_config.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  LCD Configuration — I2C mode using PCF8574
 */

#ifndef HLCD_I2C_CONFIG_H_
#define HLCD_I2C_CONFIG_H_

#include "../../MCAL/I2C/MI2C_interface.h"

/* ====================================================================
 *  LCD I2C Address (7-bit format)
 * ==================================================================== */
#define HLCD_I2C_ADDRESS    0x27U

/* ====================================================================
 *  LCD PCF8574 Pin Mapping (Bit positions in the I2C byte)
 * ==================================================================== */
#define HLCD_I2C_RS_BIT         0U
#define HLCD_I2C_RW_BIT         1U
#define HLCD_I2C_EN_BIT         2U
#define HLCD_I2C_BACKLIGHT_BIT  3U
#define HLCD_I2C_D4_BIT         4U
#define HLCD_I2C_D5_BIT         5U
#define HLCD_I2C_D6_BIT         6U
#define HLCD_I2C_D7_BIT         7U

/* State of the backlight (0 = Off, 1 = On) */
#define HLCD_I2C_BACKLIGHT_ON   (1U << HLCD_I2C_BACKLIGHT_BIT)

#endif /* HLCD_I2C_CONFIG_H_ */
