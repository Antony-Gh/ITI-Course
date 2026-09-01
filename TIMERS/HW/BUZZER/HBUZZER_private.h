/*
 * HBUZZER_private.h
 *
 *  Created on: Sep 1, 2026
 *      Author: Anthony Gaius
 */

#ifndef HBUZZER_PRIVATE_H_
#define HBUZZER_PRIVATE_H_

#include "../../CONFIG/BUZZER/HBUZZER_config.h"
#include "../../CONFIG/TIMER/MTIMER_config.h"

/*
 * OCR1A for square-wave tone via compare-match toggle.
 * freq = F_CPU / (2 * prescaler * (OCR + 1))
 */
#define HBUZZER_PRIVATE_FREQ_TO_OCR(FREQ_HZ)                                   \
	((u16)((F_CPU / (2UL * MTIMER_TIMER1_PRESCALER_VALUE * (u32)(FREQ_HZ))) - \
	       1UL))

#endif /* HBUZZER_PRIVATE_H_ */
