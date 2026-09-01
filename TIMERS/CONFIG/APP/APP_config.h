/*
 * APP_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "../../MCAL/ADC/MADC_interface.h"

/*
 * Potentiometer on ADC0 (PA0):
 *   - One leg  -> VCC (5 V)
 *   - Other leg -> GND
 *   - Wiper    -> PA0 (ADC0)
 */
#define APP_ADC_POT_CHANNEL ADC_CHANNEL_0

#define APP_ADC_MAX_VALUE 1023U
#define APP_NUM_SECTIONS 3U
#define APP_ADC_SECTION_SIZE ((APP_ADC_MAX_VALUE + 1U) / APP_NUM_SECTIONS)

/* Potentiometer sample interval in milliseconds */
#define APP_ADC_SAMPLE_MS 150U

/* ADC averaging samples per reading */
#define APP_ADC_AVERAGE_SAMPLES 4U

/* EMA filter: new_weight = 1/(2^APP_ADC_EMA_SHIFT) */
#define APP_ADC_EMA_SHIFT 4U

/* Consecutive matching samples before section changes */
#define APP_SECTION_STABLE_SAMPLES 4U

/* Hysteresis band at section boundaries (ADC counts) */
#define APP_ADC_HYSTERESIS 120U

/* Minimum time between section changes in milliseconds */
#define APP_SECTION_MIN_HOLD_MS 400U

/*
 * Invert ADC mapping: low wiper resistance (near GND, low ADC) maps to
 * the highest section (3 LEDs + highest tone).
 */
#define APP_ADC_INVERT_SECTION 1U

#endif /* APP_CONFIG_H_ */
