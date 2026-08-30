/*
 * HDCMOTOR_program.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include <util/delay.h>

#include "../../MCAL/DIO/MDIO_interface.h"

#include "HDCMOTOR_interface.h"

static u8 s_u8ActiveDirection = 0;

static void HDCMOTOR_voidSetRelayPins(u8 Copy_u8Port, u8 Copy_u8CwPin,
                                      u8 Copy_u8CcwPin, u8 Copy_u8CwState,
                                      u8 Copy_u8CcwState) {
  /* Always de-energize before energizing to avoid both relays ON. */
  if (Copy_u8CwState == HDCMOTOR_RELAY_ACTIVE_LEVEL) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin, HDCMOTOR_RELAY_ACTIVE_LEVEL);
  } else if (Copy_u8CcwState == HDCMOTOR_RELAY_ACTIVE_LEVEL) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_ACTIVE_LEVEL);
  } else {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
  }
}

void HDCMOTOR_voidInit(HDCMOTOR_t *Copy_pstMotor) {
  s_u8ActiveDirection = 0;

  if (Copy_pstMotor != (void *)0) {
    DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->Pin1,
                            DIO_OUTPUT);
    DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->Pin2,
                            DIO_OUTPUT);
    HDCMOTOR_voidSetRelayPins(
        Copy_pstMotor->Port, Copy_pstMotor->Pin1, Copy_pstMotor->Pin2,
        HDCMOTOR_RELAY_INACTIVE_LEVEL, HDCMOTOR_RELAY_INACTIVE_LEVEL);
  } else {
    DIO_enumSetPinDirection(MOTOR_PORT, MOTOR_PIN1, DIO_OUTPUT);
    DIO_enumSetPinDirection(MOTOR_PORT, MOTOR_PIN2, DIO_OUTPUT);
    HDCMOTOR_voidSetRelayPins(MOTOR_PORT, MOTOR_PIN1, MOTOR_PIN2,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL);
  }
}

void HDCMOTOR_voidRun(HDCMOTOR_t *Copy_pstMotor, u8 Copy_u8Direction) {
  u8 local_u8Port;
  u8 local_u8CwPin;
  u8 local_u8CcwPin;

  if (Copy_pstMotor == (void *)0) {
    return;
  }

  if (Copy_u8Direction != HDCMOTOR_CW && Copy_u8Direction != HDCMOTOR_CCW) {
    return;
  }

  if (Copy_u8Direction == s_u8ActiveDirection) {
    return;
  }

  local_u8Port = Copy_pstMotor->Port;
  local_u8CwPin = Copy_pstMotor->Pin1;
  local_u8CcwPin = Copy_pstMotor->Pin2;

  HDCMOTOR_voidSetRelayPins(local_u8Port, local_u8CwPin, local_u8CcwPin,
                            HDCMOTOR_RELAY_INACTIVE_LEVEL,
                            HDCMOTOR_RELAY_INACTIVE_LEVEL);

  if (s_u8ActiveDirection != 0U) {
    _delay_ms(HDCMOTOR_RELAY_SWITCH_DELAY_MS);
  }

  if (Copy_u8Direction == HDCMOTOR_CW) {
    HDCMOTOR_voidSetRelayPins(local_u8Port, local_u8CwPin, local_u8CcwPin,
                              HDCMOTOR_RELAY_ACTIVE_LEVEL,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL);
  } else {
    HDCMOTOR_voidSetRelayPins(local_u8Port, local_u8CwPin, local_u8CcwPin,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL,
                              HDCMOTOR_RELAY_ACTIVE_LEVEL);
  }

  s_u8ActiveDirection = Copy_u8Direction;
}

void HDCMOTOR_voidStop(HDCMOTOR_t *Copy_pstMotor) {
  if (Copy_pstMotor != (void *)0) {
    HDCMOTOR_voidSetRelayPins(
        Copy_pstMotor->Port, Copy_pstMotor->Pin1, Copy_pstMotor->Pin2,
        HDCMOTOR_RELAY_INACTIVE_LEVEL, HDCMOTOR_RELAY_INACTIVE_LEVEL);
  }

  s_u8ActiveDirection = 0;
}
