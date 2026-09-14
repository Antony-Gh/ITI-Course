/*
 * MTIMER_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 *
 *  Timer configuration for the Function Generator project.
 *
 *  Timer0: Reserved for DDS sample clock (configured by DDS_program.c)
 *          → NOT enabled here to avoid conflict.
 *
 *  Timer1: Reserved for hardware square wave on OC1A (configured by DDS_program.c)
 *          → NOT enabled here to avoid conflict.
 *
 *  Timer2: System tick — 1 ms CTC at 16 MHz, prescaler 64
 *          OCR2 = (16,000,000 / (64 * 1000)) - 1 = 249
 */

#ifndef MTIMER_CONFIG_H_
#define MTIMER_CONFIG_H_

#include "../../LIB/STD_TYPES.h"

/* Timer IDs */
#define MTIMER_TIMER0 0U
#define MTIMER_TIMER1 1U
#define MTIMER_TIMER2 2U

/* Timer Modes */
#define MTIMER_MODE_NORMAL 0U
#define MTIMER_MODE_CTC 1U
#define MTIMER_MODE_FAST_PWM 2U
#define MTIMER_MODE_PHASE_CORRECT_PWM 3U

/* Timer0 / Timer2 Prescaler Options */
#define MTIMER_PRESCALER_1 0U
#define MTIMER_PRESCALER_8 1U
#define MTIMER_PRESCALER_64 2U
#define MTIMER_PRESCALER_256 3U
#define MTIMER_PRESCALER_1024 4U

/* Timer1 Prescaler Options */
#define MTIMER1_PRESCALER_1 0U
#define MTIMER1_PRESCALER_8 1U
#define MTIMER1_PRESCALER_64 2U
#define MTIMER1_PRESCALER_256 3U
#define MTIMER1_PRESCALER_1024 4U

/* ====================================================================
 *  Timer0 — DISABLED (managed by DDS service)
 * ==================================================================== */
#define MTIMER_TIMER0_ENABLE 0U
#define MTIMER_TIMER0_MODE MTIMER_MODE_CTC
#define MTIMER_TIMER0_PRESCALER MTIMER_PRESCALER_1
#define MTIMER_TIMER0_OCR_VALUE 255U
#define MTIMER_TIMER0_PRESCALER_VALUE 1U
#define MTIMER_TICK_MS 1U

/* ====================================================================
 *  Timer1 — DISABLED (managed by DDS service for HW square wave)
 * ==================================================================== */
#define MTIMER_TIMER1_ENABLE 0U
#define MTIMER_TIMER1_MODE MTIMER_MODE_CTC
#define MTIMER_TIMER1_PRESCALER MTIMER1_PRESCALER_1
#define MTIMER_TIMER1_OCR1A_VALUE 0U
#define MTIMER_TIMER1_PRESCALER_VALUE 1U

/* ====================================================================
 *  Timer2 — System Tick (1 ms) for LCD, buttons, delays
 *
 *  8 MHz / 64 = 125,000 Hz timer clock
 *  125,000 / 1000 = 125 ticks per ms
 *  OCR2 = 125 - 1 = 124
 * ==================================================================== */
#define MTIMER_TIMER2_ENABLE 1U
#define MTIMER_TIMER2_MODE MTIMER_MODE_CTC
#define MTIMER_TIMER2_PRESCALER MTIMER_PRESCALER_64
#define MTIMER_TIMER2_OCR_VALUE 124U

#endif /* MTIMER_CONFIG_H_ */
