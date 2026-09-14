/*
 * DDS_private.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Private definitions for the DDS waveform engine.
 *  Not to be included outside of the DDS service.
 */

#ifndef DDS_PRIVATE_H_
#define DDS_PRIVATE_H_

#include "../../LIB/STD_TYPES.h"
#include "../../CONFIG/DDS/DDS_config.h"

/* ====================================================================
 *  Phase Accumulator Constants
 *
 *  phaseIncrement = (f_desired * 2^32) / Fs
 *
 *  We use a 64-bit intermediate to avoid overflow:
 *    temp = (u64)f_desired * 4294967296ULL
 *    phaseInc = (u32)(temp / DDS_SAMPLE_RATE)
 * ==================================================================== */
#define DDS_PHASE_ACC_FULL_SCALE  4294967296ULL   /* 2^32 */

/* MSB mask for fast square wave generation */
#define DDS_PHASE_MSB_MASK        0x80000000UL

/* ====================================================================
 *  Timer0 CTC Register Configuration (Prescaler = 1)
 *
 *  TCCR0 = WGM01 | CS00
 *  WGM01 = bit 3 → CTC mode
 *  CS00  = bit 0 → prescaler = 1 (no prescaling)
 * ==================================================================== */
#define DDS_TCCR0_CONFIG  ((1U << 3) | (1U << 0))   /* WGM01 | CS00 */

/* ====================================================================
 *  Timer1 CTC Toggle Configuration (Hardware Square Wave)
 *
 *  TCCR1A: COM1A0 = bit 6 → Toggle OC1A on compare match
 *  TCCR1B: WGM12 = bit 3 (CTC mode), CS10 = bit 0 (prescaler 1)
 * ==================================================================== */
#define DDS_TCCR1A_CONFIG  (1U << 6)                  /* COM1A0 */
#define DDS_TCCR1B_CONFIG  ((1U << 3) | (1U << 0))    /* WGM12 | CS10 */

#endif /* DDS_PRIVATE_H_ */
