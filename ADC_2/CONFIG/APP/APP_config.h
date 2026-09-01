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
 * Potentiometer on ADC0 (PA0) - standard Proteus / lab wiring:
 *   - One leg  -> VCC (5 V)
 *   - Other leg -> GND
 *   - Wiper    -> PA0 (ADC0)
 * LDR voltage divider on ADC0 (PA0) - standard Proteus / lab wiring:
 *   - LDR between VCC and PA0
 *   - 10 kOhm resistor between PA0 and GND
 *   - Brighter light -> lower LDR resistance -> higher ADC value
 */
#define APP_ADC_POT_CHANNEL ADC_CHANNEL_0
#define APP_ADC_LDR_CHANNEL ADC_CHANNEL_1

/* 10-bit ADC range split into 3 equal light sections (0-340, 341-681, 682-1023)
 */
#define APP_ADC_MAX_VALUE 1023U
#define APP_NUM_LIGHT_SECTIONS 3U
#define APP_ADC_SECTION_SIZE ((APP_ADC_MAX_VALUE + 1U) / APP_NUM_LIGHT_SECTIONS)

/*
 * Total pot resistance in ohms (Proteus POT-HG: 100 kOhm).
 * With one leg on GND and one on VCC, wiper-to-GND resistance is:
 *   R = (ADC / 1023) * APP_POT_RESISTANCE_OHMS
 */
#define APP_POT_RESISTANCE_OHMS 100000UL
#define APP_LED_SECTION_LOW_IDX 0U
#define APP_LED_SECTION_MID_IDX 1U
#define APP_LED_SECTION_HIGH_IDX 2U

/* LCD refresh interval in milliseconds */
#define APP_ADC_SAMPLE_MS 200U

#endif /* APP_CONFIG_H_ */
