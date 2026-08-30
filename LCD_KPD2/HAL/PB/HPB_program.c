/*
 * HPB_program.c
 *
 *  Created on: Aug 26, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO/MDIO_interface.h"

#include "HPB_interface.h"

void HPB_voidInit(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType) {
    /* Set the pin direction to INPUT */
    DIO_enumSetPinDirection(Copy_u8Port, Copy_u8Pin, DIO_INPUT);

    /* If Pull-Up is selected, activate the internal pull-up resistor */
    if (Copy_u8PullType == HPB_PULL_UP) {
        DIO_enumSetPinValue(Copy_u8Port, Copy_u8Pin, DIO_HIGH);
    }
}

void HPB_voidGetState(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType, u8* Copy_pu8State) {
    u8 Local_u8PinValue = 0;

    if (Copy_pu8State != NULL) {
        /* Read the physical pin value */
        DIO_enumGetPinValue(Copy_u8Port, Copy_u8Pin, &Local_u8PinValue);

        /* Determine the logical state based on the pull type */
        if (Copy_u8PullType == HPB_PULL_UP) {
            /* For Pull-Up: LOW means PRESSED, HIGH means RELEASED */
            if (Local_u8PinValue == DIO_LOW) {
                *Copy_pu8State = HPB_PRESSED;
            } else {
                *Copy_pu8State = HPB_RELEASED;
            }
        } else if (Copy_u8PullType == HPB_PULL_DOWN) {
            /* For Pull-Down: HIGH means PRESSED, LOW means RELEASED */
            if (Local_u8PinValue == DIO_HIGH) {
                *Copy_pu8State = HPB_PRESSED;
            } else {
                *Copy_pu8State = HPB_RELEASED;
            }
        }
    }
}
