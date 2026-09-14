/*
 * main.c
 *
 *  Created on : Sep 14, 2026
 *      Author : Anthony Gaius
 *
 *  ATmega32A Function / Waveform Generator
 *  Phase 5 — I2C Peripherals Integration
 *
 *  Architecture:
 *    - Timer0 CTC ISR @ 62.5 kHz → DDS phase accumulator → LUT → PORTA (DAC0808)
 *    - Timer1 CTC toggle → OC1A (PD5) hardware square wave
 *    - Timer2 CTC → 1 ms system tick
 *    - I2C Bus @ 100 kHz (SCL=PC0, SDA=PC1 in standard AVR, but ATmega32A uses SCL=PC0, SDA=PC1 for I2C)
 *      Wait, ATmega32A TWI pins are SCL = PC0, SDA = PC1.
 *    - I2C PCF8574 LCD @ 0x27
 *    - I2C PCF8574 Keypad (Rows @ 0x21, Cols @ 0x22)
 *    - I2C EEPROM 24C64 @ 0x50
 *
 *  EEPROM Memory Map:
 *    0x0000 : Waveform Type (u8)
 *    0x0001 : Frequency Byte 0 (LSB)
 *    0x0002 : Frequency Byte 1
 *    0x0003 : Frequency Byte 2
 *    0x0004 : Frequency Byte 3 (MSB)
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/interrupt.h>

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"
#include "../LIB/DELAY.h"

#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"
#include "../MCAL/I2C/MI2C_interface.h"

#include "../HAL/LCD_I2C/HLCD_I2C_interface.h"
#include "../HAL/KPD_I2C/KPD_I2C_interface.h"
#include "../HAL/EEPROM/HEEPROM_interface.h"

#include "../SERVICES/DDS/DDS_interface.h"

/* ====================================================================
 *  EEPROM Addresses
 * ==================================================================== */
#define EEPROM_ADDR_WAVEFORM    0x0000U
#define EEPROM_ADDR_FREQ_B0     0x0001U
#define EEPROM_ADDR_FREQ_B1     0x0002U
#define EEPROM_ADDR_FREQ_B2     0x0003U
#define EEPROM_ADDR_FREQ_B3     0x0004U
#define EEPROM_VALID_FLAG_ADDR  0x0005U
#define EEPROM_VALID_FLAG_VAL   0xAAU

/* ====================================================================
 *  UI Update Rate
 * ==================================================================== */
#define UI_UPDATE_PERIOD_MS     20U    /* 50 Hz main loop */
#define LCD_REFRESH_DIV         5U     /* LCD refreshed every 5th loop = 10 Hz */

/* ====================================================================
 *  Module-level variables
 * ==================================================================== */
static u8  s_u8LcdDivCounter = 0U;
static u8  s_u8DisplayDirty  = 1U;

/* Keypad numeric entry state */
static u32 s_u32TypedFreq = 0U;
static u8  s_u8IsTyping   = 0U;

/* ====================================================================
 *  EEPROM Helpers
 * ==================================================================== */
static void APP_voidSaveSettings(void)
{
    u32 local_u32Freq = DDS_u32GetFrequency();
    u8  local_u8Wave  = DDS_u8GetWaveform();

    HEEPROM_u8WriteByte(EEPROM_ADDR_WAVEFORM, local_u8Wave);
    HEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B0, (u8)((local_u32Freq >> 0)  & 0xFF));
    HEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B1, (u8)((local_u32Freq >> 8)  & 0xFF));
    HEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B2, (u8)((local_u32Freq >> 16) & 0xFF));
    HEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B3, (u8)((local_u32Freq >> 24) & 0xFF));
    
    /* Write valid flag */
    HEEPROM_u8WriteByte(EEPROM_VALID_FLAG_ADDR, EEPROM_VALID_FLAG_VAL);
}

static void APP_voidLoadSettings(void)
{
    u8 local_u8Valid = 0;
    HEEPROM_u8ReadByte(EEPROM_VALID_FLAG_ADDR, &local_u8Valid);

    if (local_u8Valid == EEPROM_VALID_FLAG_VAL) {
        u8 local_u8Wave = 0;
        u8 b0 = 0, b1 = 0, b2 = 0, b3 = 0;

        HEEPROM_u8ReadByte(EEPROM_ADDR_WAVEFORM, &local_u8Wave);
        HEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B0, &b0);
        HEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B1, &b1);
        HEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B2, &b2);
        HEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B3, &b3);

        u32 local_u32Freq = ((u32)b3 << 24) | ((u32)b2 << 16) | ((u32)b1 << 8) | (u32)b0;

        DDS_voidSetWaveform(local_u8Wave);
        DDS_voidSetFrequency(local_u32Freq);
    }
}

/* ====================================================================
 *  Private: Update LCD display
 * ==================================================================== */
static void APP_voidUpdateDisplay(void)
{
    /* Row 0: Waveform name */
    HLCD_I2C_voidGoToXY(0, 0);
    HLCD_I2C_voidSendString("WAVE: ");
    HLCD_I2C_voidSendString(DDS_pcGetWaveformName());
    HLCD_I2C_voidSendString("        "); /* Pad */

    /* Row 1: Frequency */
    HLCD_I2C_voidGoToXY(1, 0);
    
    if (s_u8IsTyping) {
        HLCD_I2C_voidSendString("Set: ");
        HLCD_I2C_voidSendNumber(s_u32TypedFreq);
        HLCD_I2C_voidSendString(" Hz_      ");
    } else {
        HLCD_I2C_voidSendString("FREQ: ");
        HLCD_I2C_voidSendFrequency(DDS_u32GetFrequency());
        HLCD_I2C_voidSendString("     ");
    }
}

/* ====================================================================
 *  Private: Splash screen on boot
 * ==================================================================== */
static void APP_voidSplashScreen(void)
{
    HLCD_I2C_voidGoToXY(0, 0);
    HLCD_I2C_voidSendString("  Function Gen  ");
    HLCD_I2C_voidGoToXY(1, 0);
    HLCD_I2C_voidSendString("  I2C Edition   ");
    DELAY_voidMs(1500);
    HLCD_I2C_voidClearScreen();
}

/* ====================================================================
 *  Main Entry Point
 * ==================================================================== */
int main(void)
{
    u8 local_u8Key;

    /* ---- Initialize DIO (configures all port directions/values) ---- */
    DIO_voidInit();

    /* ATmega32A I2C Pins: PC0=SCL, PC1=SDA. Ensure they are INPUT or Open-Drain */
    DIO_enumSetPinDirection(DIO_PORTC, DIO_PIN0, DIO_INPUT);
    DIO_enumSetPinDirection(DIO_PORTC, DIO_PIN1, DIO_INPUT);

    /* ---- Initialize Timer2 for system tick (1 ms) ---- */
    MTIMER_voidInit();

    /* ---- Initialize I2C Bus ---- */
    MI2C_voidInit();

    /* ---- Initialize I2C Peripherals ---- */
    HLCD_I2C_voidInit();
    KPD_I2C_voidInit();

    /* ---- Show splash screen ---- */
    APP_voidSplashScreen();

    /* ---- Initialize DDS engine ---- */
    DDS_voidInit();

    /* ---- Load saved settings from EEPROM ---- */
    APP_voidLoadSettings();

    /* ---- Enable global interrupts — waveform generation starts! ---- */
    sei();

    /* ---- Initial display ---- */
    APP_voidUpdateDisplay();

    u32 local_u32LastLoopMs = 0U;

    /* ====================================================================
     *  Super Loop (Non-Blocking Architecture)
     * ==================================================================== */
    while (1) {
        /* ---- Non-blocking 50 Hz UI tick (20 ms) ---- */
        if (MTIMER_u8IsIntervalElapsed(&local_u32LastLoopMs, UI_UPDATE_PERIOD_MS) == OK) {
            
            /* ---- Scan I2C Keypad ---- */
            local_u8Key = KPD_I2C_u8GetPressedKey();

            if (local_u8Key != KPD_I2C_NOT_PRESSED) {
                
                /* Numeric entry */
                if (local_u8Key >= '0' && local_u8Key <= '9') {
                    if (!s_u8IsTyping) {
                        s_u32TypedFreq = 0;
                        s_u8IsTyping = 1U;
                    }
                    /* Prevent overflow on extreme typing */
                    if (s_u32TypedFreq < 1000000UL) {
                        s_u32TypedFreq = (s_u32TypedFreq * 10) + (local_u8Key - '0');
                    }
                    s_u8DisplayDirty = 1U;
                }
                /* Enter typed frequency */
                else if (local_u8Key == 'E') {
                    if (s_u8IsTyping) {
                        DDS_voidSetFrequency(s_u32TypedFreq);
                        s_u8IsTyping = 0U;
                        APP_voidSaveSettings();
                        s_u8DisplayDirty = 1U;
                    }
                }
                /* Clear typed frequency */
                else if (local_u8Key == 'C') {
                    s_u8IsTyping = 0U;
                    s_u32TypedFreq = 0;
                    s_u8DisplayDirty = 1U;
                }
                /* Waveform cycle */
                else if (local_u8Key == 'W') {
                    DDS_voidCycleWaveform();
                    APP_voidSaveSettings();
                    s_u8DisplayDirty = 1U;
                }
                /* Explicit save */
                else if (local_u8Key == 'S') {
                    APP_voidSaveSettings();
                    
                    /* Visual feedback */
                    HLCD_I2C_voidGoToXY(1,0);
                    HLCD_I2C_voidSendString("   Saved!       ");
                    /* Keep this small visual feedback blocking for simplicity, 
                     * or use a non-blocking state machine. Since it's rare, blocking 500ms is ok */
                    DELAY_voidMs(500);
                    s_u8DisplayDirty = 1U;
                }
                /* Up / Down Stepping */
                else if (local_u8Key == 'U') {
                    DDS_voidIncrementFrequency();
                    APP_voidSaveSettings();
                    s_u8DisplayDirty = 1U;
                }
                else if (local_u8Key == 'D') {
                    DDS_voidDecrementFrequency();
                    APP_voidSaveSettings();
                    s_u8DisplayDirty = 1U;
                }
            }

            /* ---- Refresh LCD at reduced rate (10 Hz) to avoid I2C spam ---- */
            s_u8LcdDivCounter++;
            if (s_u8LcdDivCounter >= LCD_REFRESH_DIV) {
                s_u8LcdDivCounter = 0U;

                if (s_u8DisplayDirty) {
                    APP_voidUpdateDisplay();
                    s_u8DisplayDirty = 0U;
                }
            }
        }
    }

    return 0;
}
