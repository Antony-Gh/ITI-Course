/*
 * HLM35_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      Public API for the LM35 Precision Centigrade Temperature Sensor HAL driver.
 *
 *  Prerequisites:
 *      - ADC driver (MADC) must be initialised before calling any LM35 function.
 *      - LM35 VOUT connected to the ADC channel defined in HLM35_config.h.
 */

#ifndef HLM35_INTERFACE_H_
#define HLM35_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

/* Error codes */
typedef enum {
    LM35_OK = 0,
    LM35_ERR_NULL_POINTER,
    LM35_ERR_ADC_FAILURE
} LM35_ErrorStatus_t;

/**
 * @brief  Read the current temperature from the LM35 sensor.
 *
 *         Performs a blocking ADC conversion on the configured channel,
 *         then converts the raw ADC result to degrees Celsius using:
 *
 *             Temp_C = (ADC_raw × VREF_mV) / (ADC_resolution × mV_per_degree)
 *
 * @param  Copy_pu8TempC   Pointer to store the temperature in °C (integer).
 * @return LM35_OK             Successful conversion.
 *         LM35_ERR_NULL_POINTER  Copy_pu8TempC is NULL.
 *         LM35_ERR_ADC_FAILURE   ADC conversion failed or timed out.
 */
LM35_ErrorStatus_t LM35_enumReadTemperature(u8 *Copy_pu8TempC);

#endif /* HLM35_INTERFACE_H_ */
