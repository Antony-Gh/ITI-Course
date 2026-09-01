/*
 * MTIMER_interface.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef MTIMER_INTERFACE_H_
#define MTIMER_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"
#include "../../CONFIG/TIMER/MTIMER_config.h"

#define MTIMER_u8_TIMER0 0U
#define MTIMER_u8_TIMER1 1U
#define MTIMER_u8_TIMER2 2U

void MTIMER_voidInit(void);

void MTIMER_voidEnableGlobal(void);

u32 MTIMER_u32GetMillis(void);


u8 MTIMER_u8SetOverflowCallback(u8 Copy_u8TimerId,
                                void (*Copy_pvCallback)(void));


u8 MTIMER_u8SetCompareCallback(u8 Copy_u8TimerId,
                               void (*Copy_pvCallback)(void));

/*
 * Non-blocking interval check.
 * Returns OK when Copy_u16IntervalMs has elapsed since *Copy_pu32LastMs
 * and updates *Copy_pu32LastMs. Returns NOK while still waiting.
 */
u8 MTIMER_u8IsIntervalElapsed(u32 *Copy_pu32LastMs, u16 Copy_u16IntervalMs);

/* Blocking delay — spins until Copy_u16DelayMs milliseconds have passed. */
void MTIMER_voidDelayMs(u16 Copy_u16DelayMs);

#if MTIMER_TIMER1_ENABLE
void MTIMER_voidSetOcr1A(u16 Copy_u16Value);
void MTIMER_voidResetTimer1(void);
#endif

void MTIMER_voidSetCompareIntState(u8 Copy_u8TimerId, u8 Copy_u8Enable);

#endif /* MTIMER_INTERFACE_H_ */
