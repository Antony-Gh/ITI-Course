/*
 * HLCD_program.c
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 *
 *  LCD HAL driver — supports 4-bit and 8-bit modes.
 *  Mode is selected at compile time via HLCD_MODE in HLCD_config.h.
 *
 *  For the Function Generator project, 4-bit mode is used on PORTC
 *  to free PORTA for the 8-bit DAC.
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/DELAY.h"

#include "../../MCAL/DIO/MDIO_interface.h"

#include "../../CONFIG/LCD/HLCD_config.h"
#include "../../HW/LCD/HLCD_private.h"
#include "HLCD_interface.h"

/* ====================================================================
 *  Private: Send enable pulse
 * ==================================================================== */
static void HLCD_voidEnablePulse(void)
{
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_HIGH);
    DELAY_voidMs(1);
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_LOW);
    DELAY_voidMs(1);
}

/* ====================================================================
 *  Private (4-bit mode): Write upper nibble to D4..D7 pins
 * ==================================================================== */
#if (HLCD_MODE == HLCD_MODE_4BIT)
static void HLCD_voidWriteNibble(u8 Copy_u8Nibble)
{
    /* Copy_u8Nibble contains the nibble in bits 3:0.
     * Map to D4..D7 pins on HLCD_DATA_PORT. */
    DIO_enumSetPinValue(HLCD_DATA_PORT, HLCD_D4_PIN,
                        (Copy_u8Nibble >> 0) & 0x01);
    DIO_enumSetPinValue(HLCD_DATA_PORT, HLCD_D5_PIN,
                        (Copy_u8Nibble >> 1) & 0x01);
    DIO_enumSetPinValue(HLCD_DATA_PORT, HLCD_D6_PIN,
                        (Copy_u8Nibble >> 2) & 0x01);
    DIO_enumSetPinValue(HLCD_DATA_PORT, HLCD_D7_PIN,
                        (Copy_u8Nibble >> 3) & 0x01);
}
#endif

/* ====================================================================
 *  Send Command
 * ==================================================================== */
void HLCD_voidSendCommand(u8 Copy_u8Command)
{
    /* RS = 0 (command), RW = 0 (write) */
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_LOW);
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_LOW);

#if (HLCD_MODE == HLCD_MODE_4BIT)
    /* Send high nibble first */
    HLCD_voidWriteNibble((Copy_u8Command >> 4) & 0x0F);
    HLCD_voidEnablePulse();

    /* Send low nibble */
    HLCD_voidWriteNibble(Copy_u8Command & 0x0F);
    HLCD_voidEnablePulse();
#else
    /* 8-bit mode: write full byte to data port */
    DIO_enumSetPortValue(HLCD_DATA_PORT, Copy_u8Command);
    HLCD_voidEnablePulse();
#endif
}

/* ====================================================================
 *  Send Data (character)
 * ==================================================================== */
void HLCD_voidSendData(u8 Copy_u8Data)
{
    /* RS = 1 (data), RW = 0 (write) */
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_HIGH);
    DIO_enumSetPinValue(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_LOW);

#if (HLCD_MODE == HLCD_MODE_4BIT)
    /* Send high nibble first */
    HLCD_voidWriteNibble((Copy_u8Data >> 4) & 0x0F);
    HLCD_voidEnablePulse();

    /* Send low nibble */
    HLCD_voidWriteNibble(Copy_u8Data & 0x0F);
    HLCD_voidEnablePulse();
#else
    /* 8-bit mode: write full byte to data port */
    DIO_enumSetPortValue(HLCD_DATA_PORT, Copy_u8Data);
    HLCD_voidEnablePulse();
#endif
}

/* ====================================================================
 *  Initialization
 * ==================================================================== */
void HLCD_voidInit(void)
{
    /* Configure control pins as output */
    DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_RS_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_RW_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(HLCD_CTRL_PORT, HLCD_EN_PIN, DIO_OUTPUT);

#if (HLCD_MODE == HLCD_MODE_4BIT)
    /* Configure data pins D4..D7 as output */
    DIO_enumSetPinDirection(HLCD_DATA_PORT, HLCD_D4_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(HLCD_DATA_PORT, HLCD_D5_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(HLCD_DATA_PORT, HLCD_D6_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(HLCD_DATA_PORT, HLCD_D7_PIN, DIO_OUTPUT);

    /* Wait >40 ms after power-on (LCD datasheet) */
    DELAY_voidMs(50);

    /*
     * 4-bit initialization sequence (HD44780 datasheet):
     *
     * 1. Send 0x03 three times (force 8-bit mode regardless of state)
     * 2. Send 0x02 (switch to 4-bit mode)
     * 3. Then send normal commands in 4-bit protocol
     *
     * During steps 1-2, the LCD is still in 8-bit mode,
     * so we send only single nibbles via D4..D7.
     */

    /* Function set: force 8-bit mode (attempt 1) */
    HLCD_voidWriteNibble(0x03);
    HLCD_voidEnablePulse();
    DELAY_voidMs(5);

    /* Function set: force 8-bit mode (attempt 2) */
    HLCD_voidWriteNibble(0x03);
    HLCD_voidEnablePulse();
    DELAY_voidMs(1);

    /* Function set: force 8-bit mode (attempt 3) */
    HLCD_voidWriteNibble(0x03);
    HLCD_voidEnablePulse();
    DELAY_voidMs(1);

    /* Switch to 4-bit mode */
    HLCD_voidWriteNibble(0x02);
    HLCD_voidEnablePulse();
    DELAY_voidMs(1);

    /* Now in 4-bit mode — send full commands using two nibbles */
    HLCD_voidSendCommand(LCD_4BIT_2LINE_COMMAND);   /* 0x28: 4-bit, 2 lines, 5x8 */
    DELAY_voidMs(1);

#else
    /* 8-bit mode */
    DIO_enumSetPortDirection(HLCD_DATA_PORT, DIO_PORT_OUTPUT);
    DELAY_voidMs(40);
    HLCD_voidSendCommand(LCD_8BIT_2LINE_COMMAND);   /* 0x38: 8-bit, 2 lines, 5x8 */
    DELAY_voidMs(1);
#endif

    HLCD_voidSendCommand(LCD_DISPLAY_ON_COMMAND);     /* 0x0C: Display ON */
    DELAY_voidMs(1);

    HLCD_voidSendCommand(LCD_CLEAR_COMMAND);          /* 0x01: Clear */
    DELAY_voidMs(2);

    HLCD_voidSendCommand(LCD_ENTRY_MODE_SET_COMMAND); /* 0x06: Increment, no shift */
}

/* ====================================================================
 *  String / Number / Cursor Operations
 * ==================================================================== */
void HLCD_voidSendString(const char *Copy_pcString)
{
    u8 Local_u8Counter = 0;
    while (Copy_pcString[Local_u8Counter] != '\0') {
        HLCD_voidSendData(Copy_pcString[Local_u8Counter]);
        Local_u8Counter++;
    }
}

void HLCD_voidSendNumber(u32 Copy_u32Number)
{
    u8 local_au8Digits[10];
    u8 local_u8Index = 0U;

    if (Copy_u32Number == 0U) {
        HLCD_voidSendData('0');
        return;
    }

    while (Copy_u32Number > 0U) {
        local_au8Digits[local_u8Index] = (u8)(Copy_u32Number % 10U);
        Copy_u32Number /= 10U;
        local_u8Index++;
    }

    while (local_u8Index > 0U) {
        local_u8Index--;
        HLCD_voidSendData((u8)('0' + local_au8Digits[local_u8Index]));
    }
}

void HLCD_voidGoToXY(u8 Copy_u8Row, u8 Copy_u8Col)
{
    u8 Local_u8Address = 0;
    if (Copy_u8Row == 0) {
        Local_u8Address = Copy_u8Col;
    } else if (Copy_u8Row == 1) {
        Local_u8Address = Copy_u8Col + 0x40;
    }
    HLCD_voidSendCommand(LCD_SET_CURSOR_LOCATION_COMMAND | Local_u8Address);
}

void HLCD_voidClearScreen(void) { HLCD_voidSendCommand(LCD_CLEAR_COMMAND); }

void HLCD_voidSendStringTypingEffect(const char *Copy_pcString,
                                     u32 Copy_u32DelayMs)
{
    /* Turn on display with blinking cursor for typing effect */
    HLCD_voidSendCommand(LCD_DISPLAY_ON_BLINK_COMMAND);

    u8 Local_u8Counter = 0;
    while (Copy_pcString[Local_u8Counter] != '\0') {
        HLCD_voidSendData(Copy_pcString[Local_u8Counter]);

        /* Delay between characters */
        for (u32 i = 0; i < Copy_u32DelayMs; i++) {
            DELAY_voidMs(1);
        }
        Local_u8Counter++;
    }

    /* Revert to standard display (ON, Cursor OFF) */
    HLCD_voidSendCommand(LCD_DISPLAY_ON_COMMAND);
}

void HLCD_voidCreateCustomChar(const u8 *Copy_pu8Pattern, u8 Copy_u8Location)
{
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

/* ====================================================================
 *  Display formatted frequency
 *
 *  >= 1000 Hz → "X.XX kHz"  (e.g. "1.00 kHz", "12.50 kHz")
 *  <  1000 Hz → "XXX Hz"    (e.g. "100 Hz", "1 Hz")
 * ==================================================================== */
void HLCD_voidSendFrequency(u32 Copy_u32FreqHz)
{
    if (Copy_u32FreqHz >= 1000UL) {
        u32 local_u32kHz = Copy_u32FreqHz / 1000UL;
        u32 local_u32Frac = (Copy_u32FreqHz % 1000UL) / 10UL; /* Two decimal places */

        HLCD_voidSendNumber(local_u32kHz);
        HLCD_voidSendData('.');

        /* Always print two digits for fractional part */
        if (local_u32Frac < 10UL) {
            HLCD_voidSendData('0');
        }
        HLCD_voidSendNumber(local_u32Frac);
        HLCD_voidSendString(" kHz");
    } else {
        HLCD_voidSendNumber(Copy_u32FreqHz);
        HLCD_voidSendString(" Hz  ");
    }
}
