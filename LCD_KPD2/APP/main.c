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

#include "../MCAL/DIO/MDIO_interface.h"
#include "../HAL/KPD/KPD_interface.h"
#include "../HAL/LED/HLED_interface.h"
#include "LED_animations.h"

int main(void) {
    u8 local_u8Key;

    DIO_voidInit();
    KPD_voidInit();
    HLED_voidInit();

    while (1) {
        local_u8Key = KPD_u8GetPressedKey();

        if (local_u8Key != KPD_NOT_PRESSED) {
            LED_voidRunAnimation(local_u8Key);
            HLED_voidClearAll();
        }
    }

    return 0;
}
