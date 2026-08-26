/*
 * main.c
 *
 *  Created on: Aug 24, 2026
 *      Author: anthonygaius
 */

#include "BIT_MATH.h"
#include "DIO_interface.h"
#include "STD_TYPES.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif

#include <util/delay.h>

u8 SevenSeg_u8Numbers[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66,
                             0x6D, 0x7D, 0x07, 0x7F, 0x6F};

void setup() {
  DIO_voidInit();


  DIO_enumSetPortDirection(DIO_PORTA, DIO_PORT_OUTPUT);

  DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN0, DIO_OUTPUT);
  DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN1, DIO_OUTPUT);
}

void display_segments(u8 number) {
  u8 units = number % 10;
  u8 tens = number / 10;

  DIO_enumSetPinValue(DIO_PORTB, DIO_PIN0, DIO_LOW);
  DIO_enumSetPortValue(DIO_PORTA, SevenSeg_u8Numbers[tens]);
  DIO_enumSetPinValue(DIO_PORTB, DIO_PIN1, DIO_HIGH);
  _delay_ms(5);

  DIO_enumSetPinValue(DIO_PORTB, DIO_PIN1, DIO_LOW);
  DIO_enumSetPortValue(DIO_PORTA, SevenSeg_u8Numbers[units]);
  DIO_enumSetPinValue(DIO_PORTB, DIO_PIN0, DIO_HIGH);
  _delay_ms(5);
}

void loop() {
  u8 counter;
  u16 segments_cycles;

  for (counter = 0; counter < 100; counter++) {
    for (segments_cycles = 0; segments_cycles < 100; segments_cycles++) {
      display_segments(counter);
    }
  }
}

int main() {
  setup();

  while (1) {
    loop();
  }

  return 0;
}
