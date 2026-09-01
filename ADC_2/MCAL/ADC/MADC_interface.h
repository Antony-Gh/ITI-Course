/*
 * MADC_interface.h
 *
 *  Created on: Aug 31, 2026
 *      Author: Anthony Gaius
 */

#ifndef MADC_INTERFACE_H_
#define MADC_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

/* ADC Channels (ATmega32: ADC0..ADC7 on PORTA) */
#define ADC_CHANNEL_0 0U
#define ADC_CHANNEL_1 1U
#define ADC_CHANNEL_2 2U
#define ADC_CHANNEL_3 3U
#define ADC_CHANNEL_4 4U
#define ADC_CHANNEL_5 5U
#define ADC_CHANNEL_6 6U
#define ADC_CHANNEL_7 7U

/* Reference Voltage Options */
#define ADC_REF_AREF           0U
#define ADC_REF_AVCC           1U
#define ADC_REF_INTERNAL_2_56V 3U

/* Prescaler Options */
#define ADC_PRESCALER_2   0U
#define ADC_PRESCALER_4   1U
#define ADC_PRESCALER_8   2U
#define ADC_PRESCALER_16  3U
#define ADC_PRESCALER_32  4U
#define ADC_PRESCALER_64  5U
#define ADC_PRESCALER_128 6U

/* Result Alignment Options */
#define ADC_RIGHT_ADJUST 0U
#define ADC_LEFT_ADJUST  1U

/* Conversion Status */
#define ADC_CONVERSION_COMPLETE 1U
#define ADC_CONVERSION_BUSY     0U

/* Error Codes Enum */
typedef enum {
  ADC_OK = 0,
  ADC_ERR_INVALID_CHANNEL,
  ADC_ERR_NULL_POINTER,
  ADC_ERR_CONVERSION_TIMEOUT
} ADC_ErrorStatus_t;

void ADC_voidInit(void);
ADC_ErrorStatus_t ADC_enumStartConversion(u8 Copy_u8Channel);
ADC_ErrorStatus_t ADC_enumGetConversionStatus(u8 *Copy_pu8Status);
ADC_ErrorStatus_t ADC_enumReadResult(u16 *Copy_pu16Result);
ADC_ErrorStatus_t ADC_enumReadChannel(u8 Copy_u8Channel, u16 *Copy_pu16Result);
u8 ADC_u8SetCallback(void (*Copy_pvCallback)(void));

#endif /* MADC_INTERFACE_H_ */
