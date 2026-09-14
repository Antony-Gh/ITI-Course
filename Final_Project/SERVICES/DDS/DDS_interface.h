/*
 * DDS_interface.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Public API for the DDS (Direct Digital Synthesis) waveform engine.
 *
 *  The DDS service generates analog waveforms via a phase accumulator
 *  driving LUT lookups, output to an 8-bit DAC on PORTA at a fixed
 *  sample rate (Timer0 CTC ISR).
 *
 *  Additionally, Timer1 provides a jitter-free hardware square-wave
 *  output on OC1A (PD5).
 */

#ifndef DDS_INTERFACE_H_
#define DDS_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"
#include "../../CONFIG/DDS/DDS_config.h"

/* ====================================================================
 *  Initialization
 * ==================================================================== */

/*
 * Initialize the DDS engine:
 *  - Configure PORTA as output (DAC)
 *  - Configure Timer0 CTC for fixed sample rate
 *  - Configure Timer1 CTC toggle for HW square wave
 *  - Set default waveform (SINE) and frequency (1 kHz)
 *  - Enable Timer0 compare-match interrupt
 *
 * NOTE: Call sei() after this to start waveform generation.
 */
void DDS_voidInit(void);

/* ====================================================================
 *  Waveform Control
 * ==================================================================== */

/*
 * Set the active waveform type.
 *   Copy_u8WaveType: DDS_WAVE_SINE / TRIANGLE / SQUARE / SAWTOOTH / SADDLE
 */
void DDS_voidSetWaveform(u8 Copy_u8WaveType);

/*
 * Get the current waveform type.
 *   Returns: DDS_WAVE_SINE..DDS_WAVE_SADDLE
 */
u8 DDS_u8GetWaveform(void);

/*
 * Cycle to the next waveform type (wraps around).
 */
void DDS_voidCycleWaveform(void);

/*
 * Get a human-readable name string for the current waveform.
 *   Returns: pointer to a constant string (e.g. "SINE", "TRIANGLE")
 */
const char *DDS_pcGetWaveformName(void);

/* ====================================================================
 *  Frequency Control
 * ==================================================================== */

/*
 * Set the output frequency in Hz.
 *   Copy_u32FreqHz: desired frequency (clamped to DDS_FREQ_MIN..DDS_FREQ_MAX)
 *
 * Internally computes the phase increment:
 *   phaseInc = (f_desired * 2^32) / Fs
 *
 * Also updates Timer1 OCR1A for hardware square wave.
 */
void DDS_voidSetFrequency(u32 Copy_u32FreqHz);

/*
 * Get the current output frequency in Hz.
 */
u32 DDS_u32GetFrequency(void);

/*
 * Increment frequency by the appropriate step size.
 *   Step logic:
 *     1-99 Hz     → +1 Hz
 *     100-999 Hz  → +10 Hz
 *     1000-9999   → +100 Hz
 *     10000+      → +1000 Hz
 */
void DDS_voidIncrementFrequency(void);

/*
 * Decrement frequency by the appropriate step size.
 */
void DDS_voidDecrementFrequency(void);

#endif /* DDS_INTERFACE_H_ */
