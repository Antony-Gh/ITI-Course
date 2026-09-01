/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Assignment: Read potentiometer (ADC0/PA0) and LDR (ADC1/PA1) through the
 * ADC driver, display both continuously on the LCD, and drive 3 LEDs from
 * the LDR light level. LCD control on PC0..PC2, data on PORTB.
 */

#include "../LIB/STD_TYPES.h"

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/LED/HLED_interface.h"
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
  u32 local_u32ResistanceOhms =
      APP_u32CalcWiperResistanceOhms(Copy_u16AdcValue);
  HLCD_voidGoToXY(0U, 0U);
  HLCD_voidSendString("R:");
  APP_voidPrintResistanceKohm(local_u32ResistanceOhms);
  HLCD_voidGoToXY(1U, 0U);
  HLCD_voidSendString("ADC:");
  APP_voidPrintU16(Copy_u16AdcValue);
  HLCD_voidSendString("          ");
  APP_voidPrintU16(local_u16Percent);
  HLCD_voidSendString("%        ");
}

static u8 APP_u8GetLightSection(u16 Copy_u16AdcValue) {
  u8 local_u8Section = (u8)(Copy_u16AdcValue / APP_ADC_SECTION_SIZE);

  if (local_u8Section >= APP_NUM_LIGHT_SECTIONS) {
    local_u8Section = APP_NUM_LIGHT_SECTIONS - 1U;
  }

  return local_u8Section;
}

static void APP_voidUpdateSectionLeds(u8 Copy_u8Section) {
  u8 local_u8LedIdx;

  for (local_u8LedIdx = 0U; local_u8LedIdx < APP_NUM_LIGHT_SECTIONS;
       local_u8LedIdx++) {
    HLED_voidSetLed(local_u8LedIdx,
                    (local_u8LedIdx <= Copy_u8Section) ? DIO_HIGH : DIO_LOW);

    // Turn On led up to the current section
    // HLED_voidSetLed(local_u8LedIdx,
    //   (local_u8LedIdx == Copy_u8Section) ? DIO_HIGH : DIO_LOW);
  }
}

static void APP_voidDisplaySensors(u16 Copy_u16PotAdc, u16 Copy_u16LdrAdc,
                                   u8 Copy_u8Section) {
  static const char *s_apcSectionNames[APP_NUM_LIGHT_SECTIONS] = {
      "Dark", "Mid ", "High"};
  u16 local_u16PotPercent = (u16)(((u32)Copy_u16PotAdc * 100UL) / 1023UL);

  HLCD_voidGoToXY(0U, 0U);
  HLCD_voidSendString("POT:");
  APP_voidPrintU16(Copy_u16PotAdc);
  HLCD_voidSendString(" ");
  APP_voidPrintU16(local_u16PotPercent);
  HLCD_voidSendString("%        ");

  HLCD_voidGoToXY(1U, 0U);
  HLCD_voidSendString("LDR:");
  APP_voidPrintU16(Copy_u16LdrAdc);
  HLCD_voidSendString(" S:");
  APP_voidPrintU16((u16)(Copy_u8Section + 1U));
  HLCD_voidSendString(" ");
  HLCD_voidSendString(s_apcSectionNames[Copy_u8Section]);
  HLCD_voidSendString("      ");
}

static void APP_voidSampleSensors(void) {
  u16 local_u16PotAdc = 0U;
  u16 local_u16LdrAdc = 0U;
  u32 local_u32NowMs = MTIMER_u32GetMillis();

  if ((local_u32NowMs - s_u32LastSampleMs) < APP_ADC_SAMPLE_MS) {
    return;
  }

  s_u32LastSampleMs = local_u32NowMs;

  if ((ADC_enumReadChannel(APP_ADC_POT_CHANNEL, &local_u16PotAdc) == ADC_OK) &&
      (ADC_enumReadChannel(APP_ADC_LDR_CHANNEL, &local_u16LdrAdc) == ADC_OK)) {
    u8 local_u8Section = APP_u8GetLightSection(local_u16LdrAdc);

    APP_voidUpdateSectionLeds(local_u8Section);
    APP_voidDisplaySensors(local_u16PotAdc, local_u16LdrAdc, local_u8Section);
  }
}

int main(void) {
  MTIMER_voidInit();
  DIO_voidInit();
  ADC_voidInit();
  HLED_voidInit();
  HLCD_voidInit();

  HLCD_voidClearScreen();
  HLCD_voidGoToXY(0U, 0U);
  HLCD_voidSendString("Initializing...");

  s_u32LastSampleMs = MTIMER_u32GetMillis();

  while (1) {
    APP_voidSampleSensors();
  }

  return 0;
}
