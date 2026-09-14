/*
 * main.c
 *
 *  Created on : Sep 14, 2026
 *      Author : Anthony Gaius
 *
 *  ATmega32A Function / Waveform Generator
 *  ITI Embedded Systems — Final Project
 *
 *  Architecture:
 *    - Timer0 CTC ISR @ 62.5 kHz → DDS phase accumulator → LUT → PORTA (DAC0808)
 *    - Timer1 CTC toggle → OC1A (PD5) hardware square wave (jitter-free)
 *    - Timer2 CTC → 1 ms system tick for LCD, buttons, delays
 *    - 3 push-buttons: Waveform cycle (PD2), Freq+ (PD3), Freq- (PD4)
 *    - 16×2 LCD in 4-bit mode on PORTC
 *    - Amplitude control via passive potentiometer in output stage
 *
 *  Main loop runs at ~50 Hz (20 ms period):
 *    1. Poll and debounce buttons
 *    2. Check for edge events (press detection)
 *    3. Update DDS waveform/frequency as needed
 *    4. Refresh LCD display
 */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/interrupt.h>

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/PB/HPB_interface.h"

#include "../SERVICES/DDS/DDS_interface.h"

/* ====================================================================
 *  Button Definitions
 * ==================================================================== */
#define BTN_WAVE_PORT       DIO_PORTD
#define BTN_WAVE_PIN        DIO_PIN2

#define BTN_FREQ_UP_PORT    DIO_PORTD
#define BTN_FREQ_UP_PIN     DIO_PIN3

#define BTN_FREQ_DOWN_PORT  DIO_PORTD
#define BTN_FREQ_DOWN_PIN   DIO_PIN4

/* ====================================================================
 *  UI Update Rate
 * ==================================================================== */
#define UI_UPDATE_PERIOD_MS  20U    /* 50 Hz main loop */
#define LCD_REFRESH_DIV      5U     /* LCD refreshed every 5th loop = 10 Hz */

/* ====================================================================
 *  Module-level variables
 * ==================================================================== */
static HPB_t s_stBtnWave;
static HPB_t s_stBtnUp;
static HPB_t s_stBtnDown;

static u8 s_u8LcdDivCounter = 0U;
static u8 s_u8DisplayDirty  = 1U;  /* Force initial display update */

/* ====================================================================
 *  Private: Update LCD display with current waveform and frequency
 * ==================================================================== */
static void APP_voidUpdateDisplay(void)
{
    /* Row 0: Waveform name */
    HLCD_voidGoToXY(0, 0);
    HLCD_voidSendString("WAVE: ");
    HLCD_voidSendString(DDS_pcGetWaveformName());

    /* Pad with spaces to clear leftover characters */
    HLCD_voidSendString("        ");

    /* Row 1: Frequency */
    HLCD_voidGoToXY(1, 0);
    HLCD_voidSendString("FREQ: ");
    HLCD_voidSendFrequency(DDS_u32GetFrequency());

    /* Pad with spaces */
    HLCD_voidSendString("     ");
}

/* ====================================================================
 *  Private: Splash screen on boot
 * ==================================================================== */
static void APP_voidSplashScreen(void)
{
    HLCD_voidGoToXY(0, 0);
    HLCD_voidSendString("  Function Gen  ");
    HLCD_voidGoToXY(1, 0);
    HLCD_voidSendString("  ITI Project   ");
    MTIMER_voidDelayMs(1500);
    HLCD_voidClearScreen();
}

/* ====================================================================
 *  Main Entry Point
 * ==================================================================== */
int main(void)
{
    u8 local_u8Edge = HPB_EDGE_NONE;

    /* ---- Initialize DIO (configures all port directions/values) ---- */
    DIO_voidInit();

    /* ---- Initialize Timer2 for system tick (1 ms) ---- */
    MTIMER_voidInit();

    /* ---- Initialize LCD (4-bit mode on PORTC) ---- */
    HLCD_voidInit();

    /* ---- Initialize push-buttons (active-low with internal pull-up) ---- */
    HPB_enumInit(&s_stBtnWave, BTN_WAVE_PORT, BTN_WAVE_PIN, HPB_PULL_UP);
    HPB_enumInit(&s_stBtnUp,   BTN_FREQ_UP_PORT, BTN_FREQ_UP_PIN, HPB_PULL_UP);
    HPB_enumInit(&s_stBtnDown, BTN_FREQ_DOWN_PORT, BTN_FREQ_DOWN_PIN, HPB_PULL_UP);

    /* ---- Show splash screen ---- */
    APP_voidSplashScreen();

    /* ---- Initialize DDS engine ---- */
    /*   Configures Timer0 CTC for sample clock,
     *   Timer1 CTC toggle for HW square wave,
     *   PORTA as DAC output,
     *   Default: SINE @ 1 kHz */
    DDS_voidInit();

    /* ---- Enable global interrupts — waveform generation starts! ---- */
    sei();

    /* ---- Initial display ---- */
    APP_voidUpdateDisplay();

    /* ====================================================================
     *  Super Loop
     *
     *  Runs at ~50 Hz (20 ms per iteration).
     *  The waveform generation is entirely handled by the Timer0 ISR;
     *  this loop only handles user interface.
     * ==================================================================== */
    while (1) {

        /* ---- Debounce all buttons ---- */
        HPB_voidUpdate(&s_stBtnWave);
        HPB_voidUpdate(&s_stBtnUp);
        HPB_voidUpdate(&s_stBtnDown);

        /* ---- Check WAVE button (PD2) ---- */
        if (HPB_enumGetEdge(&s_stBtnWave, &local_u8Edge) == OK) {
            if (local_u8Edge == HPB_EDGE_PRESSED) {
                DDS_voidCycleWaveform();
                s_u8DisplayDirty = 1U;
            }
        }

        /* ---- Check FREQ UP button (PD3) ---- */
        if (HPB_enumGetEdge(&s_stBtnUp, &local_u8Edge) == OK) {
            if (local_u8Edge == HPB_EDGE_PRESSED) {
                DDS_voidIncrementFrequency();
                s_u8DisplayDirty = 1U;
            }
        }

        /* ---- Check FREQ DOWN button (PD4) ---- */
        if (HPB_enumGetEdge(&s_stBtnDown, &local_u8Edge) == OK) {
            if (local_u8Edge == HPB_EDGE_PRESSED) {
                DDS_voidDecrementFrequency();
                s_u8DisplayDirty = 1U;
            }
        }

        /* ---- Refresh LCD at reduced rate (10 Hz) to avoid flicker ---- */
        s_u8LcdDivCounter++;
        if (s_u8LcdDivCounter >= LCD_REFRESH_DIV) {
            s_u8LcdDivCounter = 0U;

            if (s_u8DisplayDirty) {
                APP_voidUpdateDisplay();
                s_u8DisplayDirty = 0U;
            }
        }

        /* ---- Pace the main loop ---- */
        MTIMER_voidDelayMs(UI_UPDATE_PERIOD_MS);
    }

    return 0;
}
