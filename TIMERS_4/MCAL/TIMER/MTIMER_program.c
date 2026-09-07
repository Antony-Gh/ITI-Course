/*
 * MTIMER_program.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/REGISTERS.h"



#include <avr/interrupt.h>
#include "../../HW/TIMER/MTIMER_private.h"

static volatile u32 s_u32Millis = 0U;

static void (*s_apvOverflowCallbacks)(void) = NULL;
static void (*s_apvCompareCallbacks)(void) = NULL;

void initialize(void) {
	TCCR1A = 0x00;
	TCCR1B = 0x00;
	TCNT1 = 0x0000;
	TIMSK = 0x00;
	TIFR = 0x00;

	// F_CPU = 8 MHz
	// Prescaler = 256
	// Timer frequency = 8,000,000 / 256 = 31,250 Hz
	// Timer tick = 1 / 31,250 = 32 us
	// We need 10ms for the timer
	// So = 10ms / 32us = 312.5 - 1 = 311.5
	// Then OCR1A = 311 (0 to 311 = 312 Ticks) 
	// Period = (311 + 1) * 32 us = 9.984 ms

	OCR1A = 311U;


	// CTC Mode = 0100 = 4
	CLR_BIT(TCCR1B, WGM13);
	SET_BIT(TCCR1B, WGM12);
	CLR_BIT(TCCR1A, WGM10);
	CLR_BIT(TCCR1A, WGM11);

	// Prescaler = 100 = 256
	SET_BIT(TCCR1B, CS12);
	CLR_BIT(TCCR1B, CS11);
	CLR_BIT(TCCR1B, CS10);

	// Clear Timer1 Flags
	SET_BIT(TIFR, OCF1A);


	// Enable Timer1 Compare A interrupt
	SET_BIT(TIMSK, OCIE1A);


	// Enable Global Interrupts
	SET_BIT(SREG, 7U);
}

u32 get_millis(void) {
	u32 local_u32Millis;

	CLR_BIT(SREG, 7U);
	local_u32Millis = s_u32Millis;
	SET_BIT(SREG, 7U);

	return local_u32Millis;
}

u8 is_interval_elapsed(u32 *Copy_pu32LastMs, u16 Copy_u16IntervalMs) {
	u32 local_u32NowMs;

	if (Copy_pu32LastMs == NULL) {
		return PARAM_ERR;
	}

	local_u32NowMs = get_millis();

	if ((local_u32NowMs - *Copy_pu32LastMs) < Copy_u16IntervalMs) {
		return NOK;
	}

//Copy_pu32LastMs = local_u32NowMs;
	return OK;
}

void set_overflow_callback(void (*Copy_pvCallback)(void)) {
	s_apvOverflowCallbacks = Copy_pvCallback;
}

void set_compare_callback(void (*Copy_pvCallback)(void)) {
	s_apvCompareCallbacks = Copy_pvCallback;
}

void delay_ms(u16 Copy_u16DelayMs) {
	u32 local_u32StartMs = get_millis();

	while ((get_millis() - local_u32StartMs) < Copy_u16DelayMs) {
	}
}

void reset_timer_1(void) {
	TCNT1 = 0U;
	SET_BIT(TIFR, OCF1A);
}

void set_compare_int_state(u8 Copy_u8Enable) {
	ASSIGN_BIT(TIMSK, OCIE1A, Copy_u8Enable);
}

// Timer/Counter1 Compare Match A
void __vector_7(void) __attribute__((signal));
void __vector_7(void) {

	s_u32Millis += 10U;

	
    if (s_apvCompareCallbacks != NULL) {
        s_apvCompareCallbacks();
    }
}

// Timer/Counter1 Overflow
void __vector_9(void) __attribute__((signal));
void __vector_9(void) {
    if (s_apvOverflowCallbacks != NULL) {
        s_apvOverflowCallbacks();
    }
}