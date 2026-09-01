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
 */
#define APP_ADC_POT_CHANNEL ADC_CHANNEL_0

/*
 * Total pot resistance in ohms (Proteus POT-HG: 100 kOhm).
 * With one leg on GND and one on VCC, wiper-to-GND resistance is:
 *   R = (ADC / 1023) * APP_POT_RESISTANCE_OHMS
 */
#define APP_POT_RESISTANCE_OHMS 100000UL

/* LCD refresh interval in milliseconds */
#define APP_ADC_SAMPLE_MS 200U

#endif /* APP_CONFIG_H_ */
