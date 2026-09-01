/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Assignment: Read a potentiometer through the ADC driver and display
 * the raw value and percentage on the LCD.
 * Pot wiper on PA0 (ADC0); LCD control on PC0..PC2, data on PORTB.
 */

#include "../LIB/STD_TYPES.h"

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../MCAL/ADC/MADC_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

static u32 s_u32LastSampleMs = 0U;

static void APP_voidPrintU16(u16 Copy_u16Value) {
  char local_acBuffer[6];
  u8 local_u8Index = 0U;

  if (Copy_u16Value == 0U) {
    HLCD_voidSendData('0');
    return;
  }

  while ((Copy_u16Value > 0U) && (local_u8Index < 5U)) {
    local_acBuffer[local_u8Index] = (char)('0' + (Copy_u16Value % 10U));
    Copy_u16Value /= 10U;
    local_u8Index++;
  }

  while (local_u8Index > 0U) {
    local_u8Index--;
    HLCD_voidSendData((u8)local_acBuffer[local_u8Index]);
  }
}

static u32 APP_u32CalcWiperResistanceOhms(u16 Copy_u16AdcValue) {
  return ((u32)Copy_u16AdcValue * APP_POT_RESISTANCE_OHMS) / 1023UL;
}

static void APP_voidPrintResistanceKohm(u32 Copy_u32Ohms) {
  u16 local_u16KohmWhole = (u16)(Copy_u32Ohms / 1000UL);
  u8 local_u8KohmTenth = (u8)((Copy_u32Ohms % 1000UL) / 100UL);

  APP_voidPrintU16(local_u16KohmWhole);
  HLCD_voidSendData('.');
  HLCD_voidSendData((u8)('0' + local_u8KohmTenth));
  HLCD_voidSendString("kOhm");
}

static void APP_voidDisplayAdcValue(u16 Copy_u16AdcValue) {
  u16 local_u16Percent = (u16)(((u32)Copy_u16AdcValue * 100UL) / 1023UL);
  u32 local_u32ResistanceOhms = APP_u32CalcWiperResistanceOhms(Copy_u16AdcValue);

  HLCD_voidGoToXY(0U, 0U);
  HLCD_voidSendString("R:");
  APP_voidPrintResistanceKohm(local_u32ResistanceOhms);
  HLCD_voidSendString("          ");

  HLCD_voidGoToXY(1U, 0U);
  HLCD_voidSendString("ADC:");
  APP_voidPrintU16(Copy_u16AdcValue);
  HLCD_voidSendString(" ");
  APP_voidPrintU16(local_u16Percent);
  HLCD_voidSendString("%        ");
}

static void APP_voidSamplePotentiometer(void) {
  u16 local_u16AdcValue = 0U;
  u32 local_u32NowMs = MTIMER_u32GetMillis();

  if ((local_u32NowMs - s_u32LastSampleMs) < APP_ADC_SAMPLE_MS) {
    return;
  }

  s_u32LastSampleMs = local_u32NowMs;

  if (ADC_enumReadChannel(APP_ADC_POT_CHANNEL, &local_u16AdcValue) == ADC_OK) {
    APP_voidDisplayAdcValue(local_u16AdcValue);
  }
}

int main(void) {
  MTIMER_voidInit();
  DIO_voidInit();
  ADC_voidInit();
  HLCD_voidInit();

  HLCD_voidClearScreen();
  HLCD_voidGoToXY(0U, 0U);
  HLCD_voidSendString("Initializing...");

  s_u32LastSampleMs = MTIMER_u32GetMillis();

  while (1) {
    APP_voidSamplePotentiometer();
  }

  return 0;
}
