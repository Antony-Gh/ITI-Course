/*
 * main.c
 *
 *  Created on: Aug 25, 2026
 *      Author: AnthonyGaius
 */

#include "../LIB/BIT_MATH.h"
#include "../LIB/STD_TYPES.h"

#ifndef F_CPU
#define F_CPU 8000000UL /* 8 MHz clock speed as default */
#endif

#include <util/delay.h>

#include "../HAL/LCD/HLCD_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"

// 1. Define 5x8 pixel pattern arrays
u8 SmileyFace[8] = {0b00000, 0b01010, 0b01010, 0b00000,
                    0b10001, 0b01110, 0b00000, 0b00000};

u8 HeartShape[8] = {0b00000, 0b01010, 0b11111, 0b11111,
                    0b01110, 0b00100, 0b00000, 0b00000};

u8 MusicNote[8] = {0b00001, 0b00011, 0b00101, 0b01001,
                   0b01001, 0b01011, 0b11011, 0b11000};

u8 CheckMark[8] = {0b00000, 0b00000, 0b00001, 0b00010,
                   0b10100, 0b01000, 0b00000, 0b00000};

// Location 0 : Use "\x08" Location 1 : Use "\x01" Location 2 : Use "\x02"
// Location 3 : Use "\x03" Location 4 : Use "\x04" Location 5 : Use "\x05"

void setup(void) {
  DIO_voidInit();

  HLCD_voidInit();

  _delay_ms(5000);

  // 2. Store them in CGRAM locations 0, 1, 2, and 3
  HLCD_voidCreateCustomChar(SmileyFace, 0);
  HLCD_voidCreateCustomChar(HeartShape, 1);
  HLCD_voidCreateCustomChar(MusicNote, 2);
  HLCD_voidCreateCustomChar(CheckMark, 3);

  // 3. To print them later, just send the location ID as data!
  for (u8 i = 0; i < 2; i++) {
    // Prints Smiley, Heart, Music Note, and CheckMark with a typing effect
    HLCD_voidSendStringTypingEffect("\x08 \x01 \x02 \x03 ", 250);
  }

  HLCD_voidGoToXY(1, 0);

  for (u8 i = 0; i < 2; i++) {
    HLCD_voidSendStringTypingEffect("\x08 \x01 \x02 \x03 ", 250);
  }

  _delay_ms(5000);

  HLCD_voidClearScreen();
  HLCD_voidGoToXY(0, 0);

  HLCD_voidSendStringTypingEffect("Anthony Gaius \x01", 200);
  HLCD_voidGoToXY(1, 0);
  HLCD_voidSendStringTypingEffect("EmbeddedEngineer", 200);
  
  _delay_ms(5000);

  HLCD_voidClearScreen();
  HLCD_voidSendString("Success!");
}

void loop(void) {}

int main(void) {
  setup();

  while (1) {
    loop();
  }

  return 0;
}
