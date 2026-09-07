/*
 * HLCD_program.c
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include <util/delay.h>

#include "../../MCAL/DIO/MDIO_interface.h"

#include "../../CONFIG/LCD/HLCD_config.h"
#include "../../HW/LCD/HLCD_private.h"
#include "HLCD_interface.h"

void HLCD_voidInit(void) {
    
  DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_OUTPUT);
  DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_OUTPUT);
  DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_OUTPUT);
  DIO_enumSetPortDirection(HLCD_DATA_PORT, DIO_PORT_OUTPUT);

  _delay_ms(40);

  HLCD_voidSendCommand(LCD_8BIT_2LINE_COMMAND);

  _delay_ms(1);

  HLCD_voidSendCommand(LCD_DISPLAY_ON_COMMAND);

  _delay_ms(1);

  HLCD_voidSendCommand(LCD_CLEAR_COMMAND);

  _delay_ms(2);

  HLCD_voidSendCommand(LCD_ENTRY_MODE_SET_COMMAND);
}

void HLCD_voidSendCommand(u8 Copy_u8Command) {

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_LOW);

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_LOW);

  DIO_enumSetPortValue(HLCD_DATA_PORT, Copy_u8Command);

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_HIGH);
  _delay_ms(2);
  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_LOW);
}

void HLCD_voidSendData(u8 Copy_u8Data) {
    
  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_HIGH);

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_LOW);

  DIO_enumSetPortValue(HLCD_DATA_PORT, Copy_u8Data);

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_HIGH);
  _delay_ms(2);

  DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_LOW);
}

void HLCD_voidSendString(const char *Copy_pcString) {
  u8 Local_u8Counter = 0;
  while (Copy_pcString[Local_u8Counter] != '\0') {
    HLCD_voidSendData(Copy_pcString[Local_u8Counter]);
    Local_u8Counter++;
  }
}

void HLCD_voidGoToXY(u8 Copy_u8Row, u8 Copy_u8Col) {
  u8 Local_u8Address = 0;
  if (Copy_u8Row == 0) {
    Local_u8Address = Copy_u8Col;
  } else if (Copy_u8Row == 1) {
    Local_u8Address = Copy_u8Col + 0x40;
  }
  HLCD_voidSendCommand(LCD_SET_CURSOR_LOCATION_COMMAND | Local_u8Address);
}

void HLCD_voidClearScreen(void) { HLCD_voidSendCommand(LCD_CLEAR_COMMAND); }

void HLCD_voidSendStringTypingEffect(const char *Copy_pcString, u32 Copy_u32DelayMs) {
    /* Turn on display with blinking cursor for typing effect */
    HLCD_voidSendCommand(LCD_DISPLAY_ON_BLINK_COMMAND);
    
    u8 Local_u8Counter = 0;
    while (Copy_pcString[Local_u8Counter] != '\0') {
        HLCD_voidSendData(Copy_pcString[Local_u8Counter]);
        
        /* Delay between characters */
        for (u32 i = 0; i < Copy_u32DelayMs; i++) {
            _delay_ms(1);
        }
        Local_u8Counter++;
    }
    
    /* Revert to standard display (ON, Cursor OFF) */
    HLCD_voidSendCommand(LCD_DISPLAY_ON_COMMAND);
}

void HLCD_voidCreateCustomChar(const u8 *Copy_pu8Pattern, u8 Copy_u8Location) {
    if (Copy_u8Location < 8) {
        /* Set CGRAM address (0x40 + location * 8) */
        HLCD_voidSendCommand(LCD_CGRAM_ADDRESS_COMMAND + (Copy_u8Location * 8));
        
        /* Write the 8 bytes of the pattern */
        for (u8 i = 0; i < 8; i++) {
            HLCD_voidSendData(Copy_pu8Pattern[i]);
        }
        
        /* Return to Home / DDRAM to prevent corrupting CGRAM with subsequent prints */
        HLCD_voidSendCommand(LCD_RETURN_HOME_COMMAND);
    }
}
