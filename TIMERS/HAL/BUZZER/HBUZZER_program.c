/*
 * HBUZZER_program.c
 *
 *  Created on: Sep 1, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/BUZZER/HBUZZER_config.h"
#include "../../HW/BUZZER/HBUZZER_private.h"
#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/TIMER/MTIMER_interface.h"
#include "HBUZZER_interface.h"

static const u16 s_au16ToneHz[HBUZZER_NUM_TONES] = {
	HBUZZER_TONE_SECTION0_HZ, HBUZZER_TONE_SECTION1_HZ,
	HBUZZER_TONE_SECTION2_HZ};

void HBUZZER_voidInit(void) {
	DIO_enumSetPinDirection(HBUZZER_PORT, HBUZZER_PIN, DIO_OUTPUT);
	DIO_enumSetPinValue(HBUZZER_PORT, HBUZZER_PIN, DIO_LOW);
}

void HBUZZER_voidSetSection(u8 Copy_u8Section) {
	u8 local_u8Section = Copy_u8Section;
	static u8 s_u8ActiveSection = 0xFFU;

	if (local_u8Section >= HBUZZER_NUM_TONES) {
		local_u8Section = HBUZZER_NUM_TONES - 1U;
	}

	if (local_u8Section == s_u8ActiveSection) {
		return;
	}

	s_u8ActiveSection = local_u8Section;

	MTIMER_voidStopTimer1();
	DIO_enumSetPinValue(HBUZZER_PORT, HBUZZER_PIN, DIO_HIGH);
	MTIMER_voidSetOcr1A(HBUZZER_PRIVATE_FREQ_TO_OCR(s_au16ToneHz[local_u8Section]));
	MTIMER_voidResetTimer1();
	MTIMER_voidStartTimer1();
}

void HBUZZER_voidStop(void) {
	MTIMER_voidStopTimer1();
	DIO_enumSetPinValue(HBUZZER_PORT, HBUZZER_PIN, DIO_LOW);
}
