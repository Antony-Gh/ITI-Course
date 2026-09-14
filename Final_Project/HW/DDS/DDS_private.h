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
 *  Timer0 CTC Register Configuration (Prescaler = 8)
 *
 *  TCCR0 = WGM01 | CS01
 *  WGM01 = bit 3 → CTC mode
 *  CS01  = bit 1 → prescaler = 8
 * ==================================================================== */
#define DDS_TCCR0_CONFIG  ((1U << 3) | (1U << 1))   /* WGM01 | CS01 */

/* ====================================================================
 *  Timer1 8-bit Fast PWM Configuration (PWM DAC on OC1A/PD5)
 *
 *  TCCR1A: COM1A1 = bit 7 (Clear OC1A on match, Non-inverting)
 *          WGM10  = bit 0 (Fast PWM 8-bit)
 *  TCCR1B: WGM12  = bit 3 (Fast PWM 8-bit)
 *          CS11   = bit 1 (Prescaler = 8 -> 7.8 kHz PWM)
 * ==================================================================== */
#define DDS_TCCR1A_CONFIG  ((1U << 7) | (1U << 0))    /* COM1A1 | WGM10 */
#define DDS_TCCR1B_CONFIG  ((1U << 3) | (1U << 1))    /* WGM12  | CS11  */

#endif /* DDS_PRIVATE_H_ */
