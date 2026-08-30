/*
 * MTIMER_program.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../../CONFIG/TIMER/MTIMER_config.h"
#include "../../HW/TIMER/MTIMER_private.h"
#include "MTIMER_interface.h"

static volatile u32 s_u32Millis = 0U;

ISR(TIMER0_COMP_vect) {
  s_u32Millis++;
}

void MTIMER_voidInit(void) {
  TCCR0 = (1U << WGM01) | (1U << CS01) | (1U << CS00);
  OCR0 = MTIMER_OCR0_VALUE;
  TIMSK = (1U << OCIE0);
  sei();
}

u32 MTIMER_u32GetMillis(void) {
  u32 local_u32Millis;

  cli();
  local_u32Millis = s_u32Millis;
  sei();

  return local_u32Millis;
}
