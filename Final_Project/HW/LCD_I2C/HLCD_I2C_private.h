/*
 * HLCD_I2C_private.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 */

#ifndef HLCD_I2C_PRIVATE_H_
#define HLCD_I2C_PRIVATE_H_

/* LCD Commands */
#define LCD_I2C_CLEAR_COMMAND                  0x01
#define LCD_I2C_RETURN_HOME_COMMAND            0x02
#define LCD_I2C_ENTRY_MODE_SET_COMMAND         0x06
#define LCD_I2C_DISPLAY_ON_COMMAND             0x0C
#define LCD_I2C_DISPLAY_ON_BLINK_COMMAND       0x0F
#define LCD_I2C_4BIT_2LINE_COMMAND             0x28
#define LCD_I2C_SET_CURSOR_LOCATION_COMMAND    0x80
#define LCD_I2C_CGRAM_ADDRESS_COMMAND          0x40

#endif /* HLCD_I2C_PRIVATE_H_ */
