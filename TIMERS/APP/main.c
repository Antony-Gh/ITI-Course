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
static u32 s_u32LastSectionChangeMs = 0U;
static u16 s_u16FilteredAdc = 0U;
static u8 s_u8CurrentSection = 0xFFU;
static u8 s_u8PendingSection = 0xFFU;
static u8 s_u8StableCount = 0U;

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

static u16 APP_u16FilterAdc(u16 Copy_u16NewSample) {
	u32 local_u32Filtered;

	if (s_u16FilteredAdc == 0U) {
		s_u16FilteredAdc = Copy_u16NewSample;
		return s_u16FilteredAdc;
	}

	local_u32Filtered =
	    ((u32)s_u16FilteredAdc * ((1UL << APP_ADC_EMA_SHIFT) - 1UL)) +
	    (u32)Copy_u16NewSample;
	s_u16FilteredAdc = (u16)(local_u32Filtered >> APP_ADC_EMA_SHIFT);

	return s_u16FilteredAdc;
}

static void APP_voidApplySection(u8 Copy_u8Section) {
	s_u8CurrentSection = Copy_u8Section;
	s_u32LastSectionChangeMs = MTIMER_u32GetMillis();
	HLED_voidSetSection(Copy_u8Section);
	HBUZZER_voidSetSection(Copy_u8Section);
}

static void APP_voidProcessSection(u8 Copy_u8CandidateSection) {
	u32 local_u32NowMs = MTIMER_u32GetMillis();

	if (Copy_u8CandidateSection == s_u8PendingSection) {
		if (s_u8StableCount < 255U) {
			s_u8StableCount++;
		}
	} else {
		s_u8PendingSection = Copy_u8CandidateSection;
		s_u8StableCount = 1U;
	}

	if (s_u8StableCount < APP_SECTION_STABLE_SAMPLES) {
		return;
	}

	if (Copy_u8CandidateSection == s_u8CurrentSection) {
		return;
	}

	if ((local_u32NowMs - s_u32LastSectionChangeMs) < APP_SECTION_MIN_HOLD_MS) {
		return;
	}

	APP_voidApplySection(Copy_u8CandidateSection);
}

static void APP_voidSamplePot(void) {
	u16 local_u16PotAdc = 0U;
	u16 local_u16MappedAdc = 0U;
	u8 local_u8Section;

	if (MTIMER_u8IsIntervalElapsed(&s_u32LastSampleMs, APP_ADC_SAMPLE_MS) !=
	    OK) {
		return;
	}

	if (APP_u8ReadPotAverage(&local_u16PotAdc) != OK) {
		return;
	}

	local_u16MappedAdc = APP_u16MapAdc(APP_u16FilterAdc(local_u16PotAdc));
	local_u8Section = APP_u8GetSectionWithHysteresis(local_u16MappedAdc);
	APP_voidProcessSection(local_u8Section);
}

static u32 s_u32LastHoldMs = 0U;

static void APP_voidHoldOutputs(void) {
	if (s_u8CurrentSection == 0xFFU) {
		return;
	}

	if (MTIMER_u8IsIntervalElapsed(&s_u32LastHoldMs, 20U) != OK) {
		return;
	}

	HLED_voidSetSection(s_u8CurrentSection);
}

int main(void) {
	u16 local_u16InitialAdc = 0U;
	u16 local_u16MappedAdc = 0U;

	MTIMER_voidInit();
	DIO_voidInit();
	ADC_voidInit();
	HLED_voidInit();
	HBUZZER_voidInit();

	s_u32LastSampleMs = MTIMER_u32GetMillis();
	s_u32LastSectionChangeMs = s_u32LastSampleMs;

	if (APP_u8ReadPotAverage(&local_u16InitialAdc) == OK) {
		local_u16MappedAdc =
		    APP_u16MapAdc(APP_u16FilterAdc(local_u16InitialAdc));
		APP_voidApplySection(APP_u8GetSectionWithHysteresis(local_u16MappedAdc));
		s_u8PendingSection = s_u8CurrentSection;
		s_u8StableCount = APP_SECTION_STABLE_SAMPLES;
	}

	while (1) {
		APP_voidSamplePot();
		APP_voidHoldOutputs();
	}

	return 0;
}
