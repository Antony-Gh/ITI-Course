/*
 * MADC_program.c
 *
 *  Created on: Aug 31, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#include "../../CONFIG/ADC/MADC_config.h"
#include "../../HW/ADC/MADC_private.h"
#include "MADC_interface.h"

static void (*s_pvConversionCallback)(void) = NULL;

static ADC_ErrorStatus_t ADC_enumValidateChannel(u8 Copy_u8Channel) {
  if (Copy_u8Channel > ADC_CHANNEL_7) {
    return ADC_ERR_INVALID_CHANNEL;
  }

  return ADC_OK;
}

static void ADC_voidConfigureMux(u8 Copy_u8Channel) {
  ADMUX = ADC_PRIVATE_REFS_BITS | (Copy_u8Channel & 0x0FU);

#if (ADC_RESULT_ADJUST == 1U)
  SET_BIT(ADMUX, ADLAR);
#endif
}

void ADC_voidInit(void) {
  ADC_voidConfigureMux(ADC_DEFAULT_CHANNEL);
  ADCSRA = (1U << ADEN) | ADC_PRIVATE_PRESCALER_BITS;

#if ADC_INTERRUPT_ENABLE
  SET_BIT(ADCSRA, ADIE);
#endif

  /* Allow ADC analog front-end to stabilize (datasheet). */
  _delay_us(50U);

  /* Discard the first conversion after enable. */
  SET_BIT(ADCSRA, ADSC);
  while (GET_BIT(ADCSRA, ADSC) == 1U) {
  }
}

ADC_ErrorStatus_t ADC_enumStartConversion(u8 Copy_u8Channel) {
  ADC_ErrorStatus_t Local_enumErrorState = ADC_enumValidateChannel(Copy_u8Channel);

  if (Local_enumErrorState == ADC_OK) {
    ADC_voidConfigureMux(Copy_u8Channel);
    SET_BIT(ADCSRA, ADSC);
  }

  return Local_enumErrorState;
}

ADC_ErrorStatus_t ADC_enumGetConversionStatus(u8 *Copy_pu8Status) {
  ADC_ErrorStatus_t Local_enumErrorState = ADC_OK;

  if (Copy_pu8Status == NULL) {
    Local_enumErrorState = ADC_ERR_NULL_POINTER;
  } else if (GET_BIT(ADCSRA, ADSC) == 1U) {
    *Copy_pu8Status = ADC_CONVERSION_BUSY;
  } else {
    *Copy_pu8Status = ADC_CONVERSION_COMPLETE;
  }

  return Local_enumErrorState;
}

ADC_ErrorStatus_t ADC_enumReadResult(u16 *Copy_pu16Result) {
  ADC_ErrorStatus_t Local_enumErrorState = ADC_OK;
  u8 local_u8LowByte;
  u8 local_u8HighByte;

  if (Copy_pu16Result == NULL) {
    Local_enumErrorState = ADC_ERR_NULL_POINTER;
  } else if (GET_BIT(ADCSRA, ADSC) == 1U) {
    Local_enumErrorState = ADC_ERR_CONVERSION_TIMEOUT;
  } else {
    /* ADCL must be read first on ATmega32. */
    local_u8LowByte = ADCL;
    local_u8HighByte = ADCH;

#if (ADC_RESULT_ADJUST == 1U)
    *Copy_pu16Result = ((u16)local_u8HighByte << 8) | local_u8LowByte;
#else
    *Copy_pu16Result = ((u16)(local_u8HighByte & 0x03U) << 8) | local_u8LowByte;
#endif
  }

  return Local_enumErrorState;
}

ADC_ErrorStatus_t ADC_enumReadChannel(u8 Copy_u8Channel, u16 *Copy_pu16Result) {
  ADC_ErrorStatus_t Local_enumErrorState = ADC_enumStartConversion(Copy_u8Channel);
  u32 local_u32Timeout = 0U;

  if (Local_enumErrorState != ADC_OK) {
    return Local_enumErrorState;
  }

  if (Copy_pu16Result == NULL) {
    return ADC_ERR_NULL_POINTER;
  }

  while (GET_BIT(ADCSRA, ADSC) == 1U) {
    local_u32Timeout++;

    if (local_u32Timeout > 50000UL) {
      return ADC_ERR_CONVERSION_TIMEOUT;
    }
  }

  return ADC_enumReadResult(Copy_pu16Result);
}

u8 ADC_u8SetCallback(void (*Copy_pvCallback)(void)) {
  s_pvConversionCallback = Copy_pvCallback;
  return OK;
}

#if ADC_INTERRUPT_ENABLE
ISR(ADC_vect) {
  if (s_pvConversionCallback != NULL) {
    s_pvConversionCallback();
  }
}
#endif
