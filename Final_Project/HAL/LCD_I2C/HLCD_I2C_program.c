/*
 * HLCD_I2C_program.c
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Standalone I2C LCD driver using PCF8574
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/DELAY.h"

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/I2C/MI2C_interface.h"

#include "../../CONFIG/LCD_I2C/HLCD_I2C_config.h"
#include "../../HW/LCD_I2C/HLCD_I2C_private.h"
#include "HLCD_I2C_interface.h"

static void HLCD_I2C_voidSendI2CByte(u8 Copy_u8Data)
{
    MI2C_u8Start();
    MI2C_u8SendAddress(HLCD_I2C_ADDRESS << 1);
    MI2C_u8WriteData(Copy_u8Data);
    MI2C_voidStop();
}

static void HLCD_I2C_voidWriteNibble(u8 Copy_u8Nibble, u8 Copy_u8RsState)
{
    u8 local_u8BaseByte = HLCD_I2C_BACKLIGHT_ON;

    if (Copy_u8RsState == DIO_HIGH) {
        local_u8BaseByte |= (1U << HLCD_I2C_RS_BIT);
    }

    local_u8BaseByte |= (Copy_u8Nibble << 4);

    HLCD_I2C_voidSendI2CByte(local_u8BaseByte | (1U << HLCD_I2C_EN_BIT));
    DELAY_voidMs(1);

    HLCD_I2C_voidSendI2CByte(local_u8BaseByte & ~(1U << HLCD_I2C_EN_BIT));
    DELAY_voidMs(1);
}

void HLCD_I2C_voidSendCommand(u8 Copy_u8Command)
{
    HLCD_I2C_voidWriteNibble((Copy_u8Command >> 4) & 0x0F, DIO_LOW);
    HLCD_I2C_voidWriteNibble(Copy_u8Command & 0x0F, DIO_LOW);
}

void HLCD_I2C_voidSendData(u8 Copy_u8Data)
{
    HLCD_I2C_voidWriteNibble((Copy_u8Data >> 4) & 0x0F, DIO_HIGH);
    HLCD_I2C_voidWriteNibble(Copy_u8Data & 0x0F, DIO_HIGH);
}

void HLCD_I2C_voidInit(void)
{
    DELAY_voidMs(50);

    HLCD_I2C_voidWriteNibble(0x03, DIO_LOW);
    DELAY_voidMs(5);
    HLCD_I2C_voidWriteNibble(0x03, DIO_LOW);
    DELAY_voidMs(1);
    HLCD_I2C_voidWriteNibble(0x03, DIO_LOW);
    DELAY_voidMs(1);
    HLCD_I2C_voidWriteNibble(0x02, DIO_LOW);
    DELAY_voidMs(1);

    HLCD_I2C_voidSendCommand(LCD_I2C_4BIT_2LINE_COMMAND);
    DELAY_voidMs(1);
    HLCD_I2C_voidSendCommand(LCD_I2C_DISPLAY_ON_COMMAND);
    DELAY_voidMs(1);
    HLCD_I2C_voidSendCommand(LCD_I2C_CLEAR_COMMAND);
    DELAY_voidMs(2);
    HLCD_I2C_voidSendCommand(LCD_I2C_ENTRY_MODE_SET_COMMAND);
}

void HLCD_I2C_voidSendString(const char *Copy_pcString)
{
    u8 Local_u8Counter = 0;
    while (Copy_pcString[Local_u8Counter] != '\0') {
        HLCD_I2C_voidSendData(Copy_pcString[Local_u8Counter]);
        Local_u8Counter++;
    }
}

void HLCD_I2C_voidSendNumber(u32 Copy_u32Number)
{
    u8 local_au8Digits[10];
    u8 local_u8Index = 0U;

    if (Copy_u32Number == 0U) {
        HLCD_I2C_voidSendData('0');
        return;
    }

    while (Copy_u32Number > 0U) {
        local_au8Digits[local_u8Index] = (u8)(Copy_u32Number % 10U);
        Copy_u32Number /= 10U;
        local_u8Index++;
    }

    while (local_u8Index > 0U) {
        local_u8Index--;
        HLCD_I2C_voidSendData((u8)('0' + local_au8Digits[local_u8Index]));
    }
}

void HLCD_I2C_voidGoToXY(u8 Copy_u8Row, u8 Copy_u8Col)
{
    u8 Local_u8Address = 0;
    if (Copy_u8Row == 0) {
        Local_u8Address = Copy_u8Col;
    } else if (Copy_u8Row == 1) {
        Local_u8Address = Copy_u8Col + 0x40;
    }
    HLCD_I2C_voidSendCommand(LCD_I2C_SET_CURSOR_LOCATION_COMMAND | Local_u8Address);
}

void HLCD_I2C_voidClearScreen(void) { HLCD_I2C_voidSendCommand(LCD_I2C_CLEAR_COMMAND); }

void HLCD_I2C_voidSendStringTypingEffect(const char *Copy_pcString, u32 Copy_u32DelayMs)
{
    HLCD_I2C_voidSendCommand(LCD_I2C_DISPLAY_ON_BLINK_COMMAND);
    u8 Local_u8Counter = 0;
    while (Copy_pcString[Local_u8Counter] != '\0') {
        HLCD_I2C_voidSendData(Copy_pcString[Local_u8Counter]);
        for (u32 i = 0; i < Copy_u32DelayMs; i++) {
            DELAY_voidMs(1);
        }
        Local_u8Counter++;
    }
    HLCD_I2C_voidSendCommand(LCD_I2C_DISPLAY_ON_COMMAND);
}

void HLCD_I2C_voidCreateCustomChar(const u8 *Copy_pu8Pattern, u8 Copy_u8Location)
{
    if (Copy_u8Location < 8) {
        HLCD_I2C_voidSendCommand(LCD_I2C_CGRAM_ADDRESS_COMMAND + (Copy_u8Location * 8));
        for (u8 i = 0; i < 8; i++) {
            HLCD_I2C_voidSendData(Copy_pu8Pattern[i]);
        }
        HLCD_I2C_voidSendCommand(LCD_I2C_RETURN_HOME_COMMAND);
    }
}

void HLCD_I2C_voidSendFrequency(u32 Copy_u32FreqHz)
{
    if (Copy_u32FreqHz >= 1000UL) {
        u32 local_u32kHz = Copy_u32FreqHz / 1000UL;
        u32 local_u32Frac = (Copy_u32FreqHz % 1000UL) / 10UL;

        HLCD_I2C_voidSendNumber(local_u32kHz);
        HLCD_I2C_voidSendData('.');

        if (local_u32Frac < 10UL) {
            HLCD_I2C_voidSendData('0');
        }
        HLCD_I2C_voidSendNumber(local_u32Frac);
        HLCD_I2C_voidSendString(" kHz");
    } else {
        HLCD_I2C_voidSendNumber(Copy_u32FreqHz);
        HLCD_I2C_voidSendString(" Hz  ");
    }
}
