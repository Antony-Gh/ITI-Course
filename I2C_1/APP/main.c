/*
 * main.c  –  I2C EEPROM Assignment
 *
 *  Assignment : Use I2C communication to store a value in an external
 *               EEPROM (24LC256) and retrieve it every time the
 *               microcontroller is reset.
 *
 *  Behaviour  :
 *    1. On every reset, read a "reset counter" from EEPROM address 0x0000.
 *    2. Display it on the LCD  →  "Reset Count: N"
 *    3. Increment the counter and write it back to EEPROM.
 *    4. Spin in the idle loop until the next reset.
 *
 *  Hardware   :
 *    - ATmega32  @ 8 MHz
 *    - 24LC256   on TWI (SDA = PC1, SCL = PC0)
 *      A0, A1, A2 tied to GND  →  device address = 0x50
 *    - 16×2 LCD  (configuration from HLCD_config.h)
 *
 *  Created on : Sep 10, 2026
 *      Author : Anthony Gaius
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/* ── Library headers ─────────────────────────────────────────── */
#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"

/* ── MCAL drivers ────────────────────────────────────────────── */
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

/* ── HAL drivers ─────────────────────────────────────────────── */
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/EEPROM/HEEPROM_interface.h"

/* ── EEPROM storage address for the reset counter ────────────── */
#define APP_COUNTER_ADDR    0x0000U

/* ── Sentinel: fresh / erased EEPROM cell value ─────────────── */
#define APP_EEPROM_BLANK    0xFFU

/* =========================================================== */
int main(void)
{
    u8 Local_u8Counter = 0U;
    u8 Local_u8Status  = 0U;

    /* ── 1. Peripheral initialisation ──────────────────────── */

    /* Timer must come first – MTIMER_voidDelayMs is used by LCD
     * and by the EEPROM write-cycle delay in HEEPROM_program.c  */
    MTIMER_voidInit();

    /* DIO – set port directions (LCD pins driven inside HLCD)   */
    DIO_voidInit();

    /* LCD initialisation                                         */
    HLCD_voidInit();

    /* I2C + 24LC256 EEPROM initialisation                        */
    HEEPROM_voidInit();

    /* ── 2. Read stored reset counter from EEPROM ─────────── */
    Local_u8Status = HEEPROM_u8ReadByte(APP_COUNTER_ADDR, &Local_u8Counter);

    if (Local_u8Status != HEEPROM_OK)
    {
        /* I2C / EEPROM communication failed                      */
        HLCD_voidGoToXY(0, 0);
        HLCD_voidSendString("EEPROM ERR Read");
        Local_u8Counter = 0U;   /* start fresh                   */
    }
    else
    {
        /* A blank (erased) EEPROM returns 0xFF; treat as zero    */
        if (Local_u8Counter == APP_EEPROM_BLANK)
        {
            Local_u8Counter = 0U;
        }
    }

    /* ── 3. Display the retrieved count on the LCD ─────────── */
    HLCD_voidGoToXY(0, 0);
    HLCD_voidSendString("I2C EEPROM Demo ");

    HLCD_voidGoToXY(1, 0);
    HLCD_voidSendString("Reset Count: ");
    HLCD_voidSendNumber((u32)Local_u8Counter);
    HLCD_voidSendString("  ");   /* clear stale digits            */

    /* ── 4. Increment counter and write it back to EEPROM ──── */
    Local_u8Counter++;

    Local_u8Status = HEEPROM_u8WriteByte(APP_COUNTER_ADDR, Local_u8Counter);

    if (Local_u8Status != HEEPROM_OK)
    {
        /* Write failed – warn user on second LCD row             */
        HLCD_voidGoToXY(0, 0);
        HLCD_voidSendString("EEPROM ERR Write");
    }

    /* ── 5. Idle loop ───────────────────────────────────────── */
    while (1)
    {
        /* Nothing to do – value is now safely stored in EEPROM.
         * Press the RESET button in Proteus to see the counter
         * increment on every reboot.                             */
    }

    return 0;
}
