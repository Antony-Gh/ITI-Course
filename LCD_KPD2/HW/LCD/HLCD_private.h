/*
 * HLCD_private.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#ifndef HLCD_PRIVATE_H_
#define HLCD_PRIVATE_H_

/* LCD Commands */
#define LCD_CLEAR_COMMAND                  0x01
#define LCD_RETURN_HOME_COMMAND            0x02
#define LCD_ENTRY_MODE_SET_COMMAND         0x06 /* Increment, no shift */
#define LCD_DISPLAY_ON_COMMAND             0x0C /* Display ON, Cursor OFF, Blink OFF */
#define LCD_DISPLAY_ON_BLINK_COMMAND       0x0F /* Display ON, Cursor ON, Blink ON */
#define LCD_8BIT_2LINE_COMMAND             0x38 /* 8-bit data, 2 lines, 5x8 font */
#define LCD_SET_CURSOR_LOCATION_COMMAND    0x80
#define LCD_CGRAM_ADDRESS_COMMAND          0x40

#endif /* HLCD_PRIVATE_H_ */
