/*
 * HDCMOTOR_program.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO/MDIO_interface.h"

#include "HDCMOTOR_interface.h"


void HDCMOTOR_voidInit(HDCMOTOR_t* Copy_pstMotor) {
    if (Copy_pstMotor != (void*)0) {
        DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->Pin1, DIO_OUTPUT);
        DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->Pin2, DIO_OUTPUT);
    } else {
      DIO_enumSetPinDirection(MOTOR_PORT, MOTOR_PIN1,
                              DIO_OUTPUT);
      DIO_enumSetPinDirection(MOTOR_PORT, MOTOR_PIN2,
                              DIO_OUTPUT);
    }
}

void HDCMOTOR_voidRun(HDCMOTOR_t* Copy_pstMotor, u8 Copy_u8Direction) {
    if (Copy_pstMotor != (void*)0) {
        if (Copy_u8Direction == HDCMOTOR_CW) {
            DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin1, DIO_HIGH);
            DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin2, DIO_LOW);
        } else if (Copy_u8Direction == HDCMOTOR_CCW) {
            DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin1, DIO_LOW);
            DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin2, DIO_HIGH);
        }
    }
}

void HDCMOTOR_voidStop(HDCMOTOR_t* Copy_pstMotor) {
    if (Copy_pstMotor != (void*)0) {
        DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin1, DIO_LOW);
        DIO_enumSetPinValue(Copy_pstMotor->Port, Copy_pstMotor->Pin2, DIO_LOW);
    }
}
