/*
 * DDS_config.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  DDS (Direct Digital Synthesis) configuration for the Function Generator.
 *  Timer0 CTC mode at 16 MHz / (OCR0+1) generates the fixed sample rate.
 */

#ifndef DDS_CONFIG_H_
#define DDS_CONFIG_H_

#include "../../LIB/STD_TYPES.h"

/* ====================================================================
 *  Sample Clock Configuration
 *  Timer0 CTC, Prescaler = 1, OCR0 = 255
 *  Fs = 16,000,000 / (1 * (255 + 1)) = 62,500 Hz
 * ==================================================================== */
#define DDS_SAMPLE_RATE         62500UL

/* Timer0 settings for waveform sample generation */
#define DDS_TIMER0_PRESCALER    1U
#define DDS_TIMER0_OCR_VALUE    255U

/* ====================================================================
 *  Phase Accumulator
 *  32-bit accumulator → upper 8 bits used as LUT index
 *  Frequency resolution = Fs / 2^32 ≈ 0.0000145 Hz
 * ==================================================================== */
#define DDS_PHASE_BITS          32U
#define DDS_LUT_SIZE            256U
#define DDS_LUT_INDEX_SHIFT     24U     /* 32 - 8 = 24 */

/* ====================================================================
 *  Frequency Limits
 * ==================================================================== */
#define DDS_FREQ_MIN            1UL       /* 1 Hz */
#define DDS_FREQ_MAX            20000UL   /* 20 kHz */
#define DDS_FREQ_DEFAULT        1000UL    /* 1 kHz */

/* ====================================================================
 *  Waveform Types
 * ==================================================================== */
#define DDS_WAVE_SINE           0U
#define DDS_WAVE_TRIANGLE       1U
#define DDS_WAVE_SQUARE         2U
#define DDS_WAVE_SAWTOOTH       3U
#define DDS_WAVE_SADDLE         4U
#define DDS_WAVE_COUNT          5U

/* ====================================================================
 *  PWM Output Pin (Timer1 OC1A)
 *  PD5 drives the RC Low-Pass Filter
 * ==================================================================== */
#define DDS_PWM_PORT            DIO_PORTD
#define DDS_PWM_PIN             DIO_PIN5

/* ====================================================================
 *  Frequency Step Sizes
 * ==================================================================== */
#define DDS_STEP_1HZ            1UL
#define DDS_STEP_10HZ           10UL
#define DDS_STEP_100HZ          100UL
#define DDS_STEP_1KHZ           1000UL

#endif /* DDS_CONFIG_H_ */
