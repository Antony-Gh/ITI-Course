/*
 * MADC_config.h
 *
 *  Created on: Aug 31, 2026
 *      Author: Anthony Gaius
 */

#ifndef MADC_CONFIG_H_
#define MADC_CONFIG_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

/*
 * Reference Voltage Selection
 * Options:
 *   - 0U : ADC_REF_AREF
 *   - 1U : ADC_REF_AVCC
 *   - 3U : ADC_REF_INTERNAL_2_56V
 */
#define ADC_REF_VOLTAGE 1U

/*
 * ADC Prescaler Selection
 * Target ADC clock: 50 kHz - 200 kHz (8 MHz / 64 = 125 kHz)
 * Options:
 *   - 0U : ADC_PRESCALER_2
 *   - 1U : ADC_PRESCALER_4
 *   - 2U : ADC_PRESCALER_8
 *   - 3U : ADC_PRESCALER_16
 *   - 4U : ADC_PRESCALER_32
 *   - 5U : ADC_PRESCALER_64
 *   - 6U : ADC_PRESCALER_128
 */
#define ADC_PRESCALER 5U

/*
 * Result Alignment
 * Options:
 *   - 0U : ADC_RIGHT_ADJUST
 *   - 1U : ADC_LEFT_ADJUST
 */
#define ADC_RESULT_ADJUST 0U

/* Default channel used when no channel is specified (ADC0..ADC7) */
#define ADC_DEFAULT_CHANNEL 0U

/* Set to 1 to enable ADC conversion-complete interrupt */
#define ADC_INTERRUPT_ENABLE 0U

#endif /* MADC_CONFIG_H_ */
