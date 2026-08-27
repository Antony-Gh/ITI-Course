/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif
#include <util/delay.h>

#include "../MCAL/DIO/MDIO_interface.h"
#include "../HAL/PB/HPB_interface.h"
#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"

#define BUTTON_PORT DIO_PORTB
#define BUTTON_PIN  DIO_PIN6

int main(void) {
    /* Initialize DIO */
    DIO_voidInit();
    
    /* Initialize Button (Pull-Up) */
    HPB_voidInit(BUTTON_PORT, BUTTON_PIN, HPB_PULL_DOWN);
    
    /* Initialize Motor */
    HDCMOTOR_t Motor = {MOTOR_PORT, MOTOR_PIN1, MOTOR_PIN2};
    HDCMOTOR_voidInit(&Motor);
    
    u8 local_u8ButtonState;
    
    while (1) {
        /* Read Button State */
        HPB_voidGetState(BUTTON_PORT, BUTTON_PIN, HPB_PULL_DOWN, &local_u8ButtonState);
        
        /* 
         * The switch acts as a break: motor rotates as long as switch is NOT pressed. 
         * With HPB_PULL_UP, pressed state is evaluated as HPB_PRESSED.
         */
        if (local_u8ButtonState == HPB_PRESSED) {
            /* Stop motor */
            HDCMOTOR_voidStop(&Motor);
        } else {
            /* Run motor in one direction */
            HDCMOTOR_voidRun(&Motor, HDCMOTOR_CW);
        }
    }

    return 0;
}
