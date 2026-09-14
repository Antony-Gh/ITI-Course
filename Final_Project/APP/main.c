/*
 * main.c
 *
 *  Created on : Sep 14, 2026
 *      Author : Anthony Gaius
 *
 *  ATmega32A Function / Waveform Generator
 *  Phase 7 — Hardware Simplification & Potentiometer Upgrade
 *
 *  Architecture:
 *    - Timer0 CTC ISR @ 62.5 kHz → DDS phase accumulator → LUT → OCR1A
 *    - Timer1 Fast PWM (8-bit) → OC1A (PD5) PWM DAC output
 *    - Timer2 CTC → 1 ms system tick
 *    - ADC0 (PA0) → Potentiometer for frequency sweeping
 *    - LCD (16x2) → 4-bit mode on PORTC (Data=PC4-PC7, RS=PC0, RW=PC1, EN=PC2)
 *    - Keypad (4x4) → PORTB
 *    - Internal EEPROM → Saves Waveform, Frequency, and Multiplier
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/interrupt.h>
#include <stdlib.h> /* For abs() */

#include "../LIB/BIT_MATH.h"
#include "../LIB/DELAY.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EEPROM/MEEPROM_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

#include "../HAL/KPD/KPD_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../SERVICES/DDS/DDS_interface.h"

/* ====================================================================
 *  EEPROM Addresses (Internal)
 * ==================================================================== */
#define EEPROM_ADDR_WAVEFORM 0x0010U
#define EEPROM_ADDR_FREQ_B0 0x0011U
#define EEPROM_ADDR_FREQ_B1 0x0012U
#define EEPROM_ADDR_FREQ_B2 0x0013U
#define EEPROM_ADDR_FREQ_B3 0x0014U
#define EEPROM_ADDR_MULT 0x0015U
#define EEPROM_VALID_FLAG_ADDR 0x0016U
#define EEPROM_VALID_FLAG_VAL 0xAAU

/* ====================================================================
 *  UI Update Rate
 * ==================================================================== */
#define UI_UPDATE_PERIOD_MS 20U /* 50 Hz main loop */
#define LCD_REFRESH_DIV 5U      /* LCD refreshed every 5th loop = 10 Hz */

#define ADC_HYSTERESIS 3 /* Must move pot by >3 to register */

/* ====================================================================
 *  Module-level variables
 * ==================================================================== */
static u8 s_u8LcdDivCounter = 0U;
static u8 s_u8DisplayDirty = 1U;

/* Keypad numeric entry state */
static u32 s_u32TypedFreq = 0U;
static u8 s_u8IsTyping = 0U;

static u32 s_u32Multiplier = 1UL;

/* ====================================================================
 *  EEPROM Helpers (Internal)
 * ==================================================================== */
static void APP_voidSaveSettings(void) {
  u32 local_u32Freq = DDS_u32GetFrequency();
  u8 local_u8Wave = DDS_u8GetWaveform();

  MEEPROM_u8WriteByte(EEPROM_ADDR_WAVEFORM, local_u8Wave);
  MEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B0, (u8)((local_u32Freq >> 0) & 0xFF));
  MEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B1, (u8)((local_u32Freq >> 8) & 0xFF));
  MEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B2, (u8)((local_u32Freq >> 16) & 0xFF));
  MEEPROM_u8WriteByte(EEPROM_ADDR_FREQ_B3, (u8)((local_u32Freq >> 24) & 0xFF));
  MEEPROM_u8WriteByte(EEPROM_ADDR_MULT, (u8)s_u32Multiplier);

  /* Write valid flag */
  MEEPROM_u8WriteByte(EEPROM_VALID_FLAG_ADDR, EEPROM_VALID_FLAG_VAL);
}

static void APP_voidLoadSettings(void) {
  u8 local_u8Valid = 0;
  MEEPROM_u8ReadByte(EEPROM_VALID_FLAG_ADDR, &local_u8Valid);

  if (local_u8Valid == EEPROM_VALID_FLAG_VAL) {
    u8 local_u8Wave = 0;
    u8 b0 = 0, b1 = 0, b2 = 0, b3 = 0, m = 1;

    MEEPROM_u8ReadByte(EEPROM_ADDR_WAVEFORM, &local_u8Wave);
    MEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B0, &b0);
    MEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B1, &b1);
    MEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B2, &b2);
    MEEPROM_u8ReadByte(EEPROM_ADDR_FREQ_B3, &b3);
    MEEPROM_u8ReadByte(EEPROM_ADDR_MULT, &m);

    u32 local_u32Freq =
        ((u32)b3 << 24) | ((u32)b2 << 16) | ((u32)b1 << 8) | (u32)b0;

    DDS_voidSetWaveform(local_u8Wave);
    DDS_voidSetFrequency(local_u32Freq);
    s_u32Multiplier = (m > 0) ? m : 1;
  }
}

/* ====================================================================
 *  Private: Update LCD display
 * ==================================================================== */
static void APP_voidUpdateDisplay(void) {
  /* Row 0: Waveform name & Multiplier */
  HLCD_voidGoToXY(0, 0);
  HLCD_voidSendString("W:");
  HLCD_voidSendString(DDS_pcGetWaveformName());

  /* Pad slightly, then show multiplier */
  if (s_u32Multiplier == 1UL)
    HLCD_voidSendString(" x1   ");
  else if (s_u32Multiplier == 10UL)
    HLCD_voidSendString(" x10  ");
  else
    HLCD_voidSendString(" x100 ");

  /* Row 1: Frequency */
  HLCD_voidGoToXY(1, 0);

  if (s_u8IsTyping) {
    HLCD_voidSendString("Set: ");
    HLCD_voidSendNumber(s_u32TypedFreq);
    HLCD_voidSendString(" Hz_      ");
  } else {
    HLCD_voidSendString("FREQ: ");
    HLCD_voidSendFrequency(DDS_u32GetFrequency());
    HLCD_voidSendString("     ");
  }
}

/* ====================================================================
 *  Private: Splash screen on boot
 * ==================================================================== */
static void APP_voidSplashScreen(void) {
  HLCD_voidGoToXY(0, 0);
  HLCD_voidSendString("  Function Gen  ");
  HLCD_voidGoToXY(1, 0);
  HLCD_voidSendString("  ADC Edition!  ");
  DELAY_voidMs(1500);
  HLCD_voidClearScreen();
}

/* ====================================================================
 *  Main Entry Point
 * ==================================================================== */
int main(void) {
  u8 local_u8Key;
  u32 local_u32LastLoopMs = 0U;

  /* ---- Initialize DIO (configures all port directions/values) ---- */
  DIO_voidInit();

  /* ---- Initialize Timer2 for system tick (1 ms) ---- */
  MTIMER_voidInit();

  /* ---- Initialize Peripherals ---- */
  HLCD_voidInit();
  KPD_voidInit();
  MEEPROM_voidInit();

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

  /* ====================================================================
   *  Super Loop (Non-Blocking Architecture)
   * ==================================================================== */
  while (1) {
    /* ---- Non-blocking 50 Hz UI tick (20 ms) ---- */
    if (MTIMER_u8IsIntervalElapsed(&local_u32LastLoopMs, UI_UPDATE_PERIOD_MS) ==
        OK) {

      /* ========================================================
       *  Keypad Shortcuts & Override (4x4)
       * ======================================================== */
      local_u8Key = KPD_u8GetPressedKey();

      if (local_u8Key != KPD_NOT_PRESSED) {

        /* Numeric entry (Exact override) */
        if (local_u8Key >= '0' && local_u8Key <= '9') {
          if (!s_u8IsTyping) {
            s_u32TypedFreq = 0;
            s_u8IsTyping = 1U;
          }
          if (s_u32TypedFreq < 1000000UL) {
            s_u32TypedFreq = (s_u32TypedFreq * 10) + (local_u8Key - '0');
          }
          s_u8DisplayDirty = 1U;
        }
        /* Enter exact frequency */
        else if (local_u8Key == '=') { /* Using '=' as Enter in 4x4 */
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

        /* Multiplier Toggle (Using +, -, *) */
        else if (local_u8Key == '+') {
          s_u32Multiplier = 1UL;
          s_u8DisplayDirty = 1U;
        } else if (local_u8Key == '-') {
          s_u32Multiplier = 10UL;
          s_u8DisplayDirty = 1U;
        } else if (local_u8Key == '*') {
          s_u32Multiplier = 100UL;
          s_u8DisplayDirty = 1U;
        }

        /* Waveform Cycle (Using /) */
        else if (local_u8Key == '/') {
          DDS_voidCycleWaveform();
          APP_voidSaveSettings();
          s_u8DisplayDirty = 1U;
        }
      }

      /* ========================================================
       *  LCD Refresh (10 Hz)
       * ======================================================== */
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
