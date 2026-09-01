/*
 * MTIMER_program.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../../CONFIG/TIMER/MTIMER_config.h"
#include "../../HW/TIMER/MTIMER_private.h"
#include "MTIMER_interface.h"

static volatile u32 s_u32Millis = 0U;

static void (*s_apvOverflowCallbacks[3])(void) = {NULL, NULL, NULL };
static void (*s_apvCompareCallbacks[3])(void) = {NULL, NULL, NULL };

#if MTIMER_TIMER0_ENABLE
static void MTIMER_voidInitTimer0(void) {
	TCCR0 = MTIMER_PRIVATE_TIMER0_MODE_BITS | MTIMER_PRIVATE_TIMER0_CS_BITS;
	OCR0 = MTIMER_TIMER0_OCR_VALUE;
	TCNT0 = 0U;
	SET_BIT(TIMSK, MTIMER_TIMSK_OCIE0_BIT);
}
#endif

#if MTIMER_TIMER1_ENABLE
static void MTIMER_voidInitTimer1(void) {
	TCCR1A = 0U;
	TCCR1B = MTIMER_PRIVATE_TIMER1_MODE_BITS | MTIMER_PRIVATE_TIMER1_CS_BITS;
	OCR1A = MTIMER_TIMER1_OCR1A_VALUE;
	TCNT1 = 0U;
}
#endif

#if MTIMER_TIMER2_ENABLE
static void MTIMER_voidInitTimer2(void) {
	TCCR2 = MTIMER_PRIVATE_TIMER2_MODE_BITS | MTIMER_PRIVATE_TIMER2_CS_BITS;
	OCR2 = MTIMER_TIMER2_OCR_VALUE;
	TCNT2 = 0U;
	SET_BIT(TIMSK, MTIMER_TIMSK_OCIE2_BIT);
}
#endif

void MTIMER_voidInit(void) {
#if MTIMER_TIMER0_ENABLE
	MTIMER_voidInitTimer0();
#endif

#if MTIMER_TIMER1_ENABLE
	MTIMER_voidInitTimer1();
#endif

#if MTIMER_TIMER2_ENABLE
	MTIMER_voidInitTimer2();
#endif

	MTIMER_voidEnableGlobal();
}

void MTIMER_voidEnableGlobal(void) {
	SET_BIT(SREG, 7U);
}

u32 MTIMER_u32GetMillis(void) {
	u32 local_u32Millis;

	cli();
	local_u32Millis = s_u32Millis;
	sei();

	return local_u32Millis;
}

u8 MTIMER_u8SetOverflowCallback(u8 Copy_u8TimerId,
		void (*Copy_pvCallback)(void)) {
	if (Copy_u8TimerId > MTIMER_u8_TIMER2) {
		return NOK;
	}

	s_apvOverflowCallbacks[Copy_u8TimerId] = Copy_pvCallback;
	return OK;
}

u8 MTIMER_u8SetCompareCallback(u8 Copy_u8TimerId, void (*Copy_pvCallback)(void)) {
	if (Copy_u8TimerId > MTIMER_u8_TIMER2) {
		return NOK;
	}

	s_apvCompareCallbacks[Copy_u8TimerId] = Copy_pvCallback;
	return OK;
}

u8 MTIMER_u8IsIntervalElapsed(u32 *Copy_pu32LastMs, u16 Copy_u16IntervalMs) {
	u32 local_u32NowMs;

	if (Copy_pu32LastMs == NULL) {
		return PARAM_ERR;
	}

	local_u32NowMs = MTIMER_u32GetMillis();

	if ((local_u32NowMs - *Copy_pu32LastMs) < Copy_u16IntervalMs) {
		return NOK;
	}

	*Copy_pu32LastMs = local_u32NowMs;
	return OK;
}

void MTIMER_voidDelayMs(u16 Copy_u16DelayMs) {
	u32 local_u32StartMs = MTIMER_u32GetMillis();

	while ((MTIMER_u32GetMillis() - local_u32StartMs) < Copy_u16DelayMs) {
	}
}

#if MTIMER_TIMER1_ENABLE
void MTIMER_voidSetOcr1A(u16 Copy_u16Value) {
	OCR1A = Copy_u16Value;
}

void MTIMER_voidResetTimer1(void) {
	TCNT1 = 0U;
	SET_BIT(TIFR, MTIMER_TIFR_OCF1A_BIT);
}
#endif

void MTIMER_voidSetCompareIntState(u8 Copy_u8TimerId, u8 Copy_u8Enable) {
	switch (Copy_u8TimerId) {
#if MTIMER_TIMER0_ENABLE
	case MTIMER_u8_TIMER0:
		ASSIGN_BIT(TIMSK, MTIMER_TIMSK_OCIE0_BIT, Copy_u8Enable);
		break;
#endif
#if MTIMER_TIMER1_ENABLE
	case MTIMER_u8_TIMER1:
		ASSIGN_BIT(TIMSK, MTIMER_TIMSK_OCIE1A_BIT, Copy_u8Enable);
		break;
#endif
#if MTIMER_TIMER2_ENABLE
	case MTIMER_u8_TIMER2:
		ASSIGN_BIT(TIMSK, MTIMER_TIMSK_OCIE2_BIT, Copy_u8Enable);
		break;
#endif
	default:
		break;
	}
}

#if MTIMER_TIMER0_ENABLE
ISR(TIMER0_COMP_vect) {
	s_u32Millis += MTIMER_TICK_MS;

	if (s_apvCompareCallbacks[MTIMER_u8_TIMER0] != NULL) {
		s_apvCompareCallbacks[MTIMER_u8_TIMER0]();
	}
}

ISR(TIMER0_OVF_vect) {
	if (s_apvOverflowCallbacks[MTIMER_u8_TIMER0] != NULL) {
		s_apvOverflowCallbacks[MTIMER_u8_TIMER0]();
	}
}
#endif

#if MTIMER_TIMER1_ENABLE
ISR(TIMER1_COMPA_vect) {
	if (s_apvCompareCallbacks[MTIMER_u8_TIMER1] != NULL) {
		s_apvCompareCallbacks[MTIMER_u8_TIMER1]();
	}
}

ISR(TIMER1_OVF_vect) {
	if (s_apvOverflowCallbacks[MTIMER_u8_TIMER1] != NULL) {
		s_apvOverflowCallbacks[MTIMER_u8_TIMER1]();
	}
}
#endif

#if MTIMER_TIMER2_ENABLE
ISR(TIMER2_COMP_vect) {
	if (s_apvCompareCallbacks[MTIMER_u8_TIMER2] != NULL) {
		s_apvCompareCallbacks[MTIMER_u8_TIMER2]();
	}
}

ISR(TIMER2_OVF_vect) {
	if (s_apvOverflowCallbacks[MTIMER_u8_TIMER2] != NULL) {
		s_apvOverflowCallbacks[MTIMER_u8_TIMER2]();
	}
}
#endif
