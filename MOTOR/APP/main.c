/*
 * main.c
 *
 *  Created on: Aug 25, 2026
 *      Author: AnthonyGaius
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif

#include <util/delay.h>

#include "../MCAL/DIO/MDIO_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/KPD/KPD_interface.h"

int main(void) {
    /* Initialize DIO, LCD, and KPD */
    DIO_voidInit();
    HLCD_voidInit();
    KPD_voidInit();

    HLCD_voidGoToXY(0, 0);
    HLCD_voidSendStringTypingEffect("Hello, Ahmed", 100);

    HLCD_voidGoToXY(1, 0);
    HLCD_voidSendStringTypingEffect("Type A Letter", 100);

    _delay_ms(5000);

    u8 local_u8CharCount = 0;
    u8 local_u8Key;

    /* Initial state */
    HLCD_voidClearScreen();
    HLCD_voidGoToXY(0, 0);

    while (1) {
        local_u8Key = KPD_u8GetPressedKey();
        
        if (local_u8Key != KPD_NOT_PRESSED) {
            /* If screen is full, clear it and reset counter */
            if (local_u8CharCount == 32) {
                HLCD_voidClearScreen();
                local_u8CharCount = 0;
                /* HLCD_voidClearScreen already resets cursor to 0,0 typically, 
                 * but we can explicitly set it just to be safe */
                HLCD_voidGoToXY(0, 0);
            }
            
            /* If we reached the end of the first line, move to the second line */
            if (local_u8CharCount == 16) {
                HLCD_voidGoToXY(1, 0);
            }
            
            /* Print the character on the LCD */
            HLCD_voidSendData(local_u8Key);
            
            /* Increment character count */
            local_u8CharCount++;
        }
    }

    return 0;
}
