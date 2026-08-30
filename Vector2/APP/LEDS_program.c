/*
 * LEDS_program.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#include "../LIB/STD_TYPES.h"


#include "../CONFIG/APP/LEDS_config.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "LEDS_interface.h"

static void LEDS_voidLeftToRightWrapper(void) {
  LEDS_voidLeftToRight(LEDS_ANIMATION_DURATION_MS);
}

static void LEDS_voidRightToLeftWrapper(void) {
  LEDS_voidRightToLeft(LEDS_ANIMATION_DURATION_MS);
}

static void LEDS_voidBlinkAllWrapper(void) {
  LEDS_voidBlinkAll(LEDS_ANIMATION_DURATION_MS);
}

void (*LEDS_pvAnimationArr[3])(void) = {LEDS_voidLeftToRightWrapper,
                                       LEDS_voidRightToLeftWrapper,
                                       LEDS_voidBlinkAllWrapper};

void Delay_ms(u32 Copy_u32Delay) {
  u32 Local_u32Count;
  for (Local_u32Count = 0; Local_u32Count < (Copy_u32Delay * 500); Local_u32Count++) {
    __asm__("NOP");
  }
}

void LEDS_voidLeftToRight(u16 Copy_u16DurationMs) {
  u8 Local_u8Counter;
  u16 Local_u16ElapsedTime = 0;

  while (Local_u16ElapsedTime < Copy_u16DurationMs) {
    for (Local_u8Counter = 0U; Local_u8Counter < 8U; Local_u8Counter++) {
      (void)DIO_enumSetPortValue(LEDS_PORT, (u8)(1U << Local_u8Counter));
      Delay_ms(LEDS_STEP_MS);
      Local_u16ElapsedTime += LEDS_STEP_MS;
      if (Local_u16ElapsedTime >= Copy_u16DurationMs) {
        break;
      }
    }
  }
  (void)DIO_enumSetPortValue(LEDS_PORT, 0x00U);
}

void LEDS_voidRightToLeft(u16 Copy_u16DurationMs) {
  s8 Local_s8Counter;
  u16 Local_u16ElapsedTime = 0;

  while (Local_u16ElapsedTime < Copy_u16DurationMs) {
    for (Local_s8Counter = 7; Local_s8Counter >= 0; Local_s8Counter--) {
      (void)DIO_enumSetPortValue(LEDS_PORT, (u8)(1U << Local_s8Counter));
      Delay_ms(LEDS_STEP_MS);
      Local_u16ElapsedTime += LEDS_STEP_MS;
      if (Local_u16ElapsedTime >= Copy_u16DurationMs) {
        break;
      }
    }
  }
  (void)DIO_enumSetPortValue(LEDS_PORT, 0x00U);
}

void LEDS_voidBlinkAll(u16 Copy_u16DurationMs) {
  u16 Local_u16ElapsedTime = 0;

  while (Local_u16ElapsedTime < Copy_u16DurationMs) {
    (void)DIO_enumSetPortValue(LEDS_PORT, 0xFFU);
    Delay_ms(LEDS_STEP_MS);
    Local_u16ElapsedTime += LEDS_STEP_MS;
    if (Local_u16ElapsedTime >= Copy_u16DurationMs) {
      break;
    }
    
    (void)DIO_enumSetPortValue(LEDS_PORT, 0x00U);
    Delay_ms(LEDS_STEP_MS);
    Local_u16ElapsedTime += LEDS_STEP_MS;
  }
  (void)DIO_enumSetPortValue(LEDS_PORT, 0x00U);
}
