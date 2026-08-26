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
#include "../HAL/PB/HPB_interface.h"
#include "../HAL/SSD/HSSD_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"

u8 UpperA[8] = {0b00100, 0b01010, 0b10001, 0b10001,
                0b11111, 0b10001, 0b10001, 0b00000};

u8 LowerA[8] = {0b00000, 0b00000, 0b01110, 0b00001,
                0b01111, 0b10001, 0b01111, 0b00000};

void DisplaySpecialCharecter(u8 *pattern, u8 block_num, u8 row, u8 col) {
  HLCD_voidCreateCustomChar(pattern, block_num);
  HLCD_voidGoToXY(row, col);
  HLCD_voidSendData(block_num);
}

void setup(void) {
  DIO_voidInit();

  // This Initialize the pin (D0) for the units SSD
  HSSD_voidInit();

  // Initialize the other pin (D1) for the tens SSD
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN1, DIO_OUTPUT);

  // Configure Port B Pin 6 (Mode) and Pin 7 (Increase) as Push Buttons
  HPB_voidInit(DIO_PORTB, DIO_PIN6, HPB_PULL_DOWN);
  HPB_voidInit(DIO_PORTB, DIO_PIN7, HPB_PULL_DOWN);

  HLCD_voidInit();

  HLCD_voidClearScreen();

  // My Name on the LCD In Sinusoidal Form
  u8 name[] = "ANTHONY GAIUS";
  for (u8 i = 0; i < 13; i++) {
    if (i % 2 == 0) {
      HLCD_voidGoToXY(0, i);
    } else {
      HLCD_voidGoToXY(1, i);
    }
    HLCD_voidSendData(name[i]);
  }

  _delay_ms(3000);

  HLCD_voidClearScreen();

  DisplaySpecialCharecter(UpperA, 0, 0, 5);
  DisplaySpecialCharecter(LowerA, 1, 1, 5);

  _delay_ms(3000);

  HLCD_voidClearScreen();
}

void loop(void) {
  static u8 counter = 0;
  static u8 mode = 0;
  static u8 tick = 0;
  
  static u8 last_mode_state = 0;
  static u8 last_action_state = 0;
  
  u8 current_mode_state = 0;
  u8 current_action_state = 0;
  HPB_voidGetState(DIO_PORTB, DIO_PIN6, HPB_PULL_DOWN, &current_mode_state);
  HPB_voidGetState(DIO_PORTB, DIO_PIN7, HPB_PULL_DOWN, &current_action_state);
  
  // Mode Button Logic (Rising Edge)
  if (current_mode_state == HPB_PRESSED && last_mode_state == HPB_RELEASED) {

	  // Change the mode
      mode = !mode;

      // We reset the counter and ticks on the mode change
      tick = 0;
      counter = 0;
  }
  
  last_mode_state = current_mode_state;
  
  if (mode == 1) {
      // Manual Mode = Increase on button press (Rising Edge)
      if (current_action_state == HPB_PRESSED && last_action_state == HPB_RELEASED) {
          counter++;
          if (counter > 99) counter = 0;
      }
  } else {
      // Auto Mode = Increase every 1 second (25 ticks of 40ms = 1000ms)
      tick++;
      if (tick >= 25) {
          tick = 0;
          counter++;
          if (counter > 99) counter = 0;
      }
  }
  last_action_state = current_action_state;

  // Maximum value for the u8 type
  static u8 last_disp_counter = 255;
  static u8 last_disp_mode = 255;

  if (mode != last_disp_mode) {
      HLCD_voidGoToXY(0, 1);
      if (mode == 0) {
          HLCD_voidSendString("Mode: Auto    ");
      } else {
          HLCD_voidSendString("Mode: Manual  ");
      }
      last_disp_mode = mode;
  }
  
  if (counter != last_disp_counter) {
      HLCD_voidGoToXY(1, 1);
      HLCD_voidSendString("Counter: ");
      HLCD_voidSendData((counter / 10) + '0');
      HLCD_voidSendData((counter % 10) + '0');
      HLCD_voidSendString("   ");
      last_disp_counter = counter;
  }

  // Multiplexing Display (takes 40ms total)
  u8 tens = counter / 10;
  u8 units = counter % 10;

  // Display Tens
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN0, DIO_LOW);
  HSSD_voidDisplayNumber(tens);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN1, DIO_HIGH);
  _delay_ms(20);

  // Display Units
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN1, DIO_LOW);
  HSSD_voidDisplayNumber(units);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN0, DIO_HIGH);
  _delay_ms(20);
}

int main(void) {
  setup();

  while (1) {
    loop();
  }

  return 0;
}
