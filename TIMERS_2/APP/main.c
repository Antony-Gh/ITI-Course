/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Assignment: Read potentiometer (ADC0/PA0), divide into 3 sections.
 * Each section lights more LEDs (1, 2, or 3) and plays a different
 * buzzer tone via Timer1 compare-match callback.
 */

#include "../LIB/STD_TYPES.h"

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/BUZZER/HBUZZER_interface.h"
#include "../HAL/LED/HLED_interface.h"
#include "../MCAL/ADC/MADC_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

static u32 s_u32LastSampleMs = 0U;
static u8 s_u8CurrentSection = 0xFFU;

static u16 APP_u16MapAdc(u16 Copy_u16AdcValue) {
	u16 local_u16Mapped = Copy_u16AdcValue;

#if APP_ADC_INVERT_SECTION
	local_u16Mapped = APP_ADC_MAX_VALUE - Copy_u16AdcValue;
#endif

	return local_u16Mapped;
}

static u8 APP_u8GetRawSection(u16 Copy_u16MappedAdc) {
	u8 local_u8Section =
	    (u8)(Copy_u16MappedAdc / APP_ADC_SECTION_SIZE);

	if (local_u8Section >= APP_NUM_SECTIONS) {
		local_u8Section = APP_NUM_SECTIONS - 1U;
	}

	return local_u8Section;
}

static u8 APP_u8GetSectionWithHysteresis(u16 Copy_u16MappedAdc) {
	u8 local_u8RawSection = APP_u8GetRawSection(Copy_u16MappedAdc);

	if (s_u8CurrentSection == 0xFFU) {
		return local_u8RawSection;
	}

	if (local_u8RawSection > s_u8CurrentSection) {
		u16 local_u16UpThreshold =
		    (u16)((s_u8CurrentSection + 1U) * APP_ADC_SECTION_SIZE) +
		    APP_ADC_HYSTERESIS;

		if (Copy_u16MappedAdc < local_u16UpThreshold) {
			return s_u8CurrentSection;
		}
	} else if (local_u8RawSection < s_u8CurrentSection) {
		u16 local_u16DownThreshold =
		    (u16)(s_u8CurrentSection * APP_ADC_SECTION_SIZE) -
		    APP_ADC_HYSTERESIS;

		if (Copy_u16MappedAdc > local_u16DownThreshold) {
			return s_u8CurrentSection;
		}
	}

	return local_u8RawSection;
}

static u8 APP_u8ReadPotAverage(u16 *Copy_pu16Result) {
	u32 local_u32Sum = 0U;
	u8 local_u8SampleIdx;
	u16 local_u16Sample = 0U;

	if (Copy_pu16Result == NULL) {
		return NOK;
	}

	for (local_u8SampleIdx = 0U; local_u8SampleIdx < APP_ADC_AVERAGE_SAMPLES;
	     local_u8SampleIdx++) {
		if (ADC_enumReadChannel(APP_ADC_POT_CHANNEL, &local_u16Sample) !=
		    ADC_OK) {
			return NOK;
		}
		local_u32Sum += local_u16Sample;
	}

	*Copy_pu16Result = (u16)(local_u32Sum / APP_ADC_AVERAGE_SAMPLES);
	return OK;
}

static void APP_voidApplySection(u8 Copy_u8Section) {
	HLED_voidSetSection(Copy_u8Section);
	HBUZZER_voidSetSection(Copy_u8Section);
}

static void APP_voidSamplePot(void) {
	u16 local_u16PotAdc = 0U;
	u16 local_u16MappedAdc = 0U;
	u8 local_u8Section;

	if (MTIMER_u8IsIntervalElapsed(&s_u32LastSampleMs, APP_ADC_SAMPLE_MS) !=
	    OK) {
		return;
	}

	local_u16PotAdc = 0U;
	if (APP_u8ReadPotAverage(&local_u16PotAdc) != OK) {
		return;
	}

	local_u16MappedAdc = APP_u16MapAdc(local_u16PotAdc);
	local_u8Section = APP_u8GetSectionWithHysteresis(local_u16MappedAdc);

	if (local_u8Section != s_u8CurrentSection) {
		s_u8CurrentSection = local_u8Section;
		APP_voidApplySection(local_u8Section);
	}
}

int main(void) {
	u16 local_u16InitialAdc = 0U;

	MTIMER_voidInit();
	DIO_voidInit();
	ADC_voidInit();
	HLED_voidInit();
	HBUZZER_voidInit();

	s_u32LastSampleMs = MTIMER_u32GetMillis();

	local_u16InitialAdc = 0U;
	if (APP_u8ReadPotAverage(&local_u16InitialAdc) == OK) {
		s_u8CurrentSection = APP_u8GetSectionWithHysteresis(
		    APP_u16MapAdc(local_u16InitialAdc));
		APP_voidApplySection(s_u8CurrentSection);
	}

	while (1) {
		APP_voidSamplePot();
	}

	return 0;
}
