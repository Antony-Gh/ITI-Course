/*
 * MEXTI_program.c
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include <avr/interrupt.h>
#include <avr/io.h>

#include "../../CONFIG/EXTI/MEXTI_config.h"
#include "../../HW/EXTI/MEXTI_private.h"
#include "MEXTI_interface.h"

static void (*s_apvCallbacks[3])(void) = {NULL, NULL, NULL};

static void EXTI_voidConfigureInt0Sense(u8 Copy_u8Sense) {
  switch (Copy_u8Sense) {
  case EXTI_LOW_LEVEL:
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC01_BIT);
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC00_BIT);
    break;
  case EXTI_ANY_CHANGE:
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC01_BIT);
    SET_BIT(MCUCR, EXTI_MCUCR_ISC00_BIT);
    break;
  case EXTI_FALLING_EDGE:
    SET_BIT(MCUCR, EXTI_MCUCR_ISC01_BIT);
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC00_BIT);
    break;
  case EXTI_RISING_EDGE:
  default:
    SET_BIT(MCUCR, EXTI_MCUCR_ISC01_BIT);
    SET_BIT(MCUCR, EXTI_MCUCR_ISC00_BIT);
    break;
  }
}

static void EXTI_voidConfigureInt1Sense(u8 Copy_u8Sense) {
  switch (Copy_u8Sense) {
  case EXTI_LOW_LEVEL:
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC11_BIT);
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC10_BIT);
    break;
  case EXTI_ANY_CHANGE:
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC11_BIT);
    SET_BIT(MCUCR, EXTI_MCUCR_ISC10_BIT);
    break;
  case EXTI_FALLING_EDGE:
    SET_BIT(MCUCR, EXTI_MCUCR_ISC11_BIT);
    CLR_BIT(MCUCR, EXTI_MCUCR_ISC10_BIT);
    break;
  case EXTI_RISING_EDGE:
  default:
    SET_BIT(MCUCR, EXTI_MCUCR_ISC11_BIT);
    SET_BIT(MCUCR, EXTI_MCUCR_ISC10_BIT);
    break;
  }
}

static void EXTI_voidConfigureInt2Sense(u8 Copy_u8Sense) {
  if (Copy_u8Sense == EXTI_RISING_EDGE) {
    SET_BIT(MCUCSR, EXTI_MCUCSR_ISC2_BIT);
  } else {
    CLR_BIT(MCUCSR, EXTI_MCUCSR_ISC2_BIT);
  }
}

void EXTI_voidInit(void) {
#if EXTI_INT0_ENABLE
  EXTI_voidConfigureInt0Sense(EXTI_INT0_SENSE);
  SET_BIT(GIFR, EXTI_GIFR_INTF0_BIT);
  SET_BIT(GICR, EXTI_GICR_INT0_BIT);
#endif

#if EXTI_INT1_ENABLE
  EXTI_voidConfigureInt1Sense(EXTI_INT1_SENSE);
  SET_BIT(GIFR, EXTI_GIFR_INTF1_BIT);
  SET_BIT(GICR, EXTI_GICR_INT1_BIT);
#endif

#if EXTI_INT2_ENABLE
  EXTI_voidConfigureInt2Sense(EXTI_INT2_SENSE);
  SET_BIT(GIFR, EXTI_GIFR_INTF2_BIT);
  SET_BIT(GICR, EXTI_GICR_INT2_BIT);
#endif
}

void EXTI_voidEnableGlobal(void) {
  SET_BIT(SREG, 7U);
}

u8 EXTI_u8SetCallback(u8 Copy_u8IntId, void (*Copy_pvCallback)(void)) {
  if (Copy_u8IntId > EXTI_u8_INT2) {
    return NOK;
  }

  s_apvCallbacks[Copy_u8IntId] = Copy_pvCallback;
  return OK;
}

#if EXTI_INT0_ENABLE
ISR(INT0_vect) {
  if (s_apvCallbacks[EXTI_u8_INT0] != NULL) {
    s_apvCallbacks[EXTI_u8_INT0]();
  }
}
#endif

#if EXTI_INT1_ENABLE
ISR(INT1_vect) {
  if (s_apvCallbacks[EXTI_u8_INT1] != NULL) {
    s_apvCallbacks[EXTI_u8_INT1]();
  }
}
#endif

#if EXTI_INT2_ENABLE
ISR(INT2_vect) {
  if (s_apvCallbacks[EXTI_u8_INT2] != NULL) {
    s_apvCallbacks[EXTI_u8_INT2]();
  }
}
#endif
