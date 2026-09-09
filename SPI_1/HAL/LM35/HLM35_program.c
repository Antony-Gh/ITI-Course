/*
 * HLM35_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      LM35 temperature sensor HAL driver implementation.
 *
 *  Formula used:
 *      LM35 outputs 10 mV per °C.
 *      Using AVCC = 5 V reference and 10-bit ADC (0–1023):
 *
 *          Voltage (mV) = (ADC_raw * 5000) / 1024
 *          Temp    (°C) = Voltage_mV / 10
 *
 *      Combined: Temp_C = (ADC_raw * 5000) / (1024 * 10)
 *                       = (ADC_raw * 500)  / 1024
 *
 *      Integer arithmetic only – no floating point needed.
 */

#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/ADC/MADC_interface.h"
#include "../../CONFIG/LM35/HLM35_config.h"

#include "HLM35_interface.h"

LM35_ErrorStatus_t LM35_enumReadTemperature(u8 *Copy_pu8TempC) {
    u16 local_u16AdcRaw   = 0U;
    u32 local_u32TempCalc = 0U;

    ADC_ErrorStatus_t local_enumAdcErr;

    if (Copy_pu8TempC == NULL) {
        return LM35_ERR_NULL_POINTER;
    }

    /* Perform a blocking ADC conversion on the LM35 channel */
    local_enumAdcErr = ADC_enumReadChannel(LM35_ADC_CHANNEL, &local_u16AdcRaw);
    if (local_enumAdcErr != ADC_OK) {
        return LM35_ERR_ADC_FAILURE;
    }

    /*
     * Convert ADC reading to temperature in °C (integer arithmetic):
     *
     *   Temp_C = (ADC_raw × VREF_mV) / (ADC_resolution × mV_per_degree)
     *
     * All intermediate values fit in u32 (max: 1023 × 5000 = 5,115,000 < 2^32).
     */
    local_u32TempCalc = ((u32)local_u16AdcRaw * (u32)LM35_VREF_MV) /
                        ((u32)LM35_ADC_RESOLUTION * (u32)LM35_MV_PER_DEGREE);

    *Copy_pu8TempC = (u8)local_u32TempCalc;

    return LM35_OK;
}
