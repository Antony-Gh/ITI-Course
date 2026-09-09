/*
 * HLM35_config.h
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      Compile-time configuration for the LM35 temperature sensor HAL driver.
 *
 *  Sensor Notes:
 *      The LM35 outputs 10 mV per °C.
 *      At 5V AVCC reference (ADC 10-bit):
 *          Resolution = 5000 mV / 1024 = 4.8828 mV/step
 *          Temp (°C)  = (ADC_raw × 5000) / (1024 × 10)
 *                     = (ADC_raw × 500)  / 1024
 *
 *  Wiring:
 *      LM35 VCC  → +5V
 *      LM35 GND  → GND
 *      LM35 VOUT → PA0 (ADC Channel 0)
 */

#ifndef HLM35_CONFIG_H_
#define HLM35_CONFIG_H_

#include "../../MCAL/ADC/MADC_interface.h"

/* ADC channel the LM35 output (VOUT) is connected to */
#define LM35_ADC_CHANNEL   ADC_CHANNEL_0

/*
 * ADC Reference Voltage in millivolts.
 * Must match the ADC_REF_VOLTAGE setting in MADC_config.h.
 *   ADC_REF_AVCC          → 5000 mV
 *   ADC_REF_INTERNAL_2_56V→ 2560 mV
 */
#define LM35_VREF_MV       5000U

/*
 * LM35 output scale factor: 10 mV/°C
 * Do not change unless using a different sensor.
 */
#define LM35_MV_PER_DEGREE 10U

/*
 * ADC resolution (10-bit → 1024 steps on ATmega32)
 */
#define LM35_ADC_RESOLUTION 1024U

#endif /* HLM35_CONFIG_H_ */
