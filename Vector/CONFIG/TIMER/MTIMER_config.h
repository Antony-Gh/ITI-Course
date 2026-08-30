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

/* Timer0 CTC: 1 ms tick at 8 MHz with prescaler 64 */
#define MTIMER_TICK_MS 1U
#define MTIMER_PRESCALER 64U
#define MTIMER_OCR0_VALUE 124U

#endif /* MTIMER_CONFIG_H_ */
