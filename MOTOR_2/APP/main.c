/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Application: 2-Relay H-Bridge DC Motor Direction Control
 *
 * Hardware:
 *   PB6 -> OFF button  (toggle motor power)
 *   PB7 -> MODE button (toggle direction while running)
 *   PD0 -> NPN transistor -> Relay K1 (CW direction)
 *   PD1 -> NPN transistor -> Relay K2 (CCW direction)
 *
 * The relays form an H-Bridge that reverses motor polarity.
 * Transistors isolate the AVR from the relay coil / motor supply.
 */

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif
#include <util/delay.h>

#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../HAL/PB/HPB_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"

#define OFF_BUTTON_PORT DIO_PORTB
#define OFF_BUTTON_PIN DIO_PIN6

#define MODE_BUTTON_PORT DIO_PORTB
#define MODE_BUTTON_PIN DIO_PIN7

int main(void) {
  /* Initialize DIO */
  DIO_voidInit();

  /* Initialize Button (Pull-Down) */
  HPB_voidInit(OFF_BUTTON_PORT, OFF_BUTTON_PIN, HPB_PULL_DOWN);

  HPB_voidInit(MODE_BUTTON_PORT, MODE_BUTTON_PIN, HPB_PULL_DOWN);

  /* Initialize 2-relay H-bridge motor driver (PD0=CW relay, PD1=CCW relay) */
  HDCMOTOR_t Motor = {HDCMOTOR_RELAY_PORT, HDCMOTOR_RELAY_CW_PIN,
                      HDCMOTOR_RELAY_CCW_PIN};
  HDCMOTOR_voidInit(&Motor);

  u8 local_u8ButtonState;
  u8 local_u8PreviousButtonState = HPB_RELEASED;
  u8 local_u8PowerState = 0; /* 0 = OFF, 1 = ON */

  u8 local_u8Mode;
  u8 local_u8PreviousMode = HPB_RELEASED;
  u8 local_u8CurrentDirection = HDCMOTOR_CW;

  while (1) {
    /* Read Button States */
    HPB_voidGetState(OFF_BUTTON_PORT, OFF_BUTTON_PIN, HPB_PULL_DOWN,
                     &local_u8ButtonState);

    HPB_voidGetState(MODE_BUTTON_PORT, MODE_BUTTON_PIN, HPB_PULL_DOWN,
                     &local_u8Mode);

    /* Edge detection for OFF Button (Toggle Power) */
    if (local_u8ButtonState == HPB_PRESSED && local_u8PreviousButtonState == HPB_RELEASED) {
      if (local_u8PowerState == 1) {
        local_u8PowerState = 0; /* Turn off */
        local_u8CurrentDirection = HDCMOTOR_CW; /* Reset mode to forward */
      } else {
        local_u8PowerState = 1; /* Turn on */
      }
    }
    local_u8PreviousButtonState = local_u8ButtonState;

    /* Edge detection for Mode Button (toggle direction) */
    if (local_u8Mode == HPB_PRESSED && local_u8PreviousMode == HPB_RELEASED) {
      if (local_u8CurrentDirection == HDCMOTOR_CW) {
        local_u8CurrentDirection = HDCMOTOR_CCW;
      } else {
        local_u8CurrentDirection = HDCMOTOR_CW;
      }
    }
    local_u8PreviousMode = local_u8Mode;

    /* Motor Control Logic */
    if (local_u8PowerState == 0) {
      /* Stop motor */
      HDCMOTOR_voidStop(&Motor);
    } else {
      /* Run motor in current direction */
      HDCMOTOR_voidRun(&Motor, local_u8CurrentDirection);
    }

    /* Small delay for debounce */
    _delay_ms(10);
  }

  return 0;
}
