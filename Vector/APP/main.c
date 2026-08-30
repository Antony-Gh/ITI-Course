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
#include "../MCAL/DIO/MDIO_interface.h"

int main(void) {
  DIO_voidInit();
  (void)DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN2, DIO_INPUT);
  (void)DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN2, DIO_OUTPUT);

  // Enable internal pull-up on INT2 (PB2) - Don't Use External pull-down
  (void)DIO_enumSetPinValue(DIO_PORTB, DIO_PIN2, DIO_HIGH);
  // Turn off led at the start
  (void)DIO_enumSetPinValue(DIO_PORTA, DIO_PIN2, DIO_LOW);
  // Clear any pending INT2 flag
  // GIFR = General Interrupt Flag Register
  // INTF2 = Interrupt Flag 2 = 5
  SET_BIT(GIFR, 5);
  // Set INT2 trigger to falling edge (0 = failing / 1 = Rising)
  // MCUCSR = MCU Control and Status Register
  // ISC2 = Interrupt Sense Control 2 = 0
  CLR_BIT(MCUCSR, 0);
  // Enable INT2 interrupt
  // GICR = General Interrupt Control Register
  // INT2 = External Interrupt Request 2 Enable = 5
  SET_BIT(GICR, 5);
  // Enable global interrupts (I-bit)
  // SREG = Status Register
  // I = Global Interrupt Enable = Bit 7
  SET_BIT(SREG, 7);

  /* Empty infinite loop */
  while (1) {}

  return 0;
}

// INT2 interrupt service routine
void __vector_3(void) __attribute__((signal));
void __vector_3(void) {
  static u8 flag = 0U;

  if (flag == 0U) {
    (void)DIO_enumSetPinValue(DIO_PORTA, DIO_PIN2, DIO_HIGH);
    flag = 1U;
  } else {
    (void)DIO_enumSetPinValue(DIO_PORTA, DIO_PIN2, DIO_LOW);
    flag = 0U;
  }
}
