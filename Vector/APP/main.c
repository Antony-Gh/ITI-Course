/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Application: INT2 external interrupt toggles LED on PA2
 */

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../MCAL/DIO/MDIO_interface.h"

int main(void) {
  /* Initialize DIO ports */
  DIO_voidInit();

  /* INT2 pin (PB2) as input */
  (void)DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN2, DIO_INPUT);

  /* LED pin (PA2) as output */
  (void)DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN2, DIO_OUTPUT);

  /* Enable pull-up on INT2 (PB2) */
  (void)DIO_enumSetPinValue(DIO_PORTB, DIO_PIN2, DIO_HIGH);

  /* Set INT2 trigger to falling edge */
  CLR_BIT(MCUCSR, 6);

  /* Enable INT2 specific interrupt */
  SET_BIT(GICR, 5);

  /* Enable Global Interrupts (I-bit) */
  SET_BIT(SREG, 7);

  /* Empty infinite loop */
  while (1) {
  }

  return 0;
}

ISR(INT2_vect) {
  /* Static flag to track LED state */
  static u8 flag = 0U;

  if (flag == 0U) {
    /* Turn LED ON */
    (void)DIO_enumSetPinValue(DIO_PORTA, DIO_PIN2, DIO_HIGH);
    /* Update flag state */
    flag = 1U;
  } else {
    /* Turn LED OFF */
    (void)DIO_enumSetPinValue(DIO_PORTA, DIO_PIN2, DIO_LOW);
    /* Update flag state */
    flag = 0U;
  }
}
