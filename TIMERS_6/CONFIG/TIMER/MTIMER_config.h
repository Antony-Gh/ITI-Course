/*
 * MTIMER_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef MTIMER_CONFIG_H_
#define MTIMER_CONFIG_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

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

/*
 * Timer0 1 ms tick (CTC, 8 MHz, prescaler 64)
 * OCR0 = (F_CPU / (prescaler * 1000)) - 1 = 124
 * Application uses this tick to generate 50 Hz / 45% PWM on PB3.
 */
#define MTIMER_TIMER0_ENABLE 1U
#define MTIMER_TIMER0_MODE MTIMER_MODE_CTC
#define MTIMER_TIMER0_PRESCALER MTIMER_PRESCALER_64
/*
     With an 8 MHz CPU and a prescaler of 64, the timer frequency is 125 kHz (1
   tick = 8 µs). To get a 1 ms tick, you need 125 ticks. Setting OCR0 to 124
   means the timer counts from 0 to 124 (125 steps), taking exactly 1 ms.
*/
#define MTIMER_TIMER0_OCR_VALUE 124U
#define MTIMER_TIMER0_PRESCALER_VALUE 64U
#define MTIMER_TICK_MS 1U

/*
 * Timer1 free-running timebase for software ICU (Normal mode).
 * Tick = 8 * (1 / 8 MHz) = 1 us
 */
#define MTIMER_TIMER1_ENABLE 1U
#define MTIMER_TIMER1_MODE MTIMER_MODE_NORMAL
#define MTIMER_TIMER1_PRESCALER MTIMER1_PRESCALER_8
#define MTIMER_TIMER1_OCR1A_VALUE 0U
#define MTIMER_TIMER1_PRESCALER_VALUE 8U

/* Timer2 disabled by default */
#define MTIMER_TIMER2_ENABLE 0U
#define MTIMER_TIMER2_MODE MTIMER_MODE_NORMAL
#define MTIMER_TIMER2_PRESCALER MTIMER_PRESCALER_64
#define MTIMER_TIMER2_OCR_VALUE 0U

#endif /* MTIMER_CONFIG_H_ */
