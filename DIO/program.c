/*
 * main.c
 *
 *  Created on: Aug 24, 2026
 *      Author: anthonygaius
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "DIO_interface.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif

#include <util/delay.h>

u8 SevenSeg_u8Numbers[10] = {
    0x3F,
    0x06,
    0x5B,
    0x4F,
    0x66,
    0x6D,
    0x7D,
    0x07,
    0x7F,
    0x6F
};

void setup() {
    DIO_voidInit();
    DIO_enumSetPortDirection(DIO_PORTA, DIO_PORT_OUTPUT);
}

void loop() {
    u8 Local_u8Counter;

    for (Local_u8Counter = 0; Local_u8Counter < 10; Local_u8Counter++)
    {
        DIO_enumSetPortValue(DIO_PORTA, SevenSeg_u8Numbers[Local_u8Counter]);
        _delay_ms(1000);
    }
}

int main() {
    setup();

    while(1){
        loop();
    }

    return 0;
}
