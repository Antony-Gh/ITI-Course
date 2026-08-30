/*
 * main.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 *
 */

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include <avr/io.h>

#include "../CONFIG/APP/LEDS_config.h"
#include "LEDS_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"

int main(void) {

  DIO_voidInit();
  EXTI_voidInit();

  (void)DIO_enumSetPortDirection(LEDS_PORT, DIO_PORT_OUTPUT);
  (void)DIO_enumSetPortValue(LEDS_PORT, 0x00U);

  // INT0 pin (PD2) as input with pull-up
  (void)DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
  (void)DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

  // INT1 pin (PD3) as input with pull-up
  (void)DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN3, DIO_INPUT);
  (void)DIO_enumSetPinValue(DIO_PORTD, DIO_PIN3, DIO_HIGH);

  // INT2 pin (PB2) as input with pull-up
  (void)DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN2, DIO_INPUT);
  (void)DIO_enumSetPinValue(DIO_PORTB, DIO_PIN2, DIO_HIGH);

  // INT0 = Left to Right
  // INT1 = Right to Left
  // INT2 = Blink All
  // LEDS_pvAnimationArr is an array of function pointers to the animation functions
  (void)EXTI_u8SetCallback(EXTI_u8_INT0, LEDS_pvAnimationArr[0]);
  (void)EXTI_u8SetCallback(EXTI_u8_INT1, LEDS_pvAnimationArr[1]);
  (void)EXTI_u8SetCallback(EXTI_u8_INT2, LEDS_pvAnimationArr[2]);

  /* Enable global interrupts (I-bit) */
  SET_BIT(SREG, 7U);

  while (1) {
    // Empty Infinite Loop, because everything happens in the ISRs
  }

  return 0;
}
