/*
 * HPB_program.c
 *
 *  Created on: Aug 26, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO/MDIO_interface.h"

#include "HPB_interface.h"

static u8 HPB_u8ReadLogicalState(u8 Copy_u8Port, u8 Copy_u8Pin,
                                 u8 Copy_u8PullType) {
  u8 local_u8PinValue = DIO_LOW;

  DIO_enumGetPinValue(Copy_u8Port, Copy_u8Pin, &local_u8PinValue);

  if (Copy_u8PullType == HPB_PULL_UP) {
    return (local_u8PinValue == DIO_LOW) ? HPB_PRESSED : HPB_RELEASED;
  }

  return (local_u8PinValue == DIO_HIGH) ? HPB_PRESSED : HPB_RELEASED;
}

STD_err_e HPB_enumInit(HPB_t *Copy_pstButton, u8 Copy_u8Port, u8 Copy_u8Pin,
                       u8 Copy_u8PullType) {
  if (Copy_pstButton == NULL) {
    return PARAM_ERR;
  }

  Copy_pstButton->Port = Copy_u8Port;
  Copy_pstButton->Pin = Copy_u8Pin;
  Copy_pstButton->PullType = Copy_u8PullType;
  Copy_pstButton->StableCount = 0U;
  Copy_pstButton->LastStableRaw = HPB_RELEASED;
  Copy_pstButton->DebouncedState = HPB_RELEASED;
  Copy_pstButton->PreviousDebouncedState = HPB_RELEASED;

  DIO_enumSetPinDirection(Copy_u8Port, Copy_u8Pin, DIO_INPUT);

  if (Copy_u8PullType == HPB_PULL_UP) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8Pin, DIO_HIGH);
  }

  return OK;
}

void HPB_voidUpdate(HPB_t *Copy_pstButton) {
  u8 local_u8RawState;

  if (Copy_pstButton == NULL) {
    return;
  }

  local_u8RawState =
      HPB_u8ReadLogicalState(Copy_pstButton->Port, Copy_pstButton->Pin,
                             Copy_pstButton->PullType);

  if (local_u8RawState == Copy_pstButton->LastStableRaw) {
    if (Copy_pstButton->StableCount < HPB_DEBOUNCE_SAMPLES) {
      Copy_pstButton->StableCount++;
    }
  } else {
    Copy_pstButton->LastStableRaw = local_u8RawState;
    Copy_pstButton->StableCount = 1U;
  }

  if (Copy_pstButton->StableCount >= HPB_DEBOUNCE_SAMPLES) {
    Copy_pstButton->DebouncedState = local_u8RawState;
  }
}

STD_err_e HPB_enumGetState(const HPB_t *Copy_pstButton, u8 *Copy_pu8State) {
  if (Copy_pstButton == NULL || Copy_pu8State == NULL) {
    return PARAM_ERR;
  }

  *Copy_pu8State = Copy_pstButton->DebouncedState;
  return OK;
}

STD_err_e HPB_enumGetEdge(HPB_t *Copy_pstButton, u8 *Copy_pu8Edge) {
  if (Copy_pstButton == NULL || Copy_pu8Edge == NULL) {
    return PARAM_ERR;
  }

  *Copy_pu8Edge = HPB_EDGE_NONE;

  if (Copy_pstButton->DebouncedState != Copy_pstButton->PreviousDebouncedState) {
    if (Copy_pstButton->DebouncedState == HPB_PRESSED) {
      *Copy_pu8Edge = HPB_EDGE_PRESSED;
    } else {
      *Copy_pu8Edge = HPB_EDGE_RELEASED;
    }

    Copy_pstButton->PreviousDebouncedState = Copy_pstButton->DebouncedState;
  }

  return OK;
}

void HPB_voidInit(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType) {
  DIO_enumSetPinDirection(Copy_u8Port, Copy_u8Pin, DIO_INPUT);

  if (Copy_u8PullType == HPB_PULL_UP) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8Pin, DIO_HIGH);
  }
}

void HPB_voidGetState(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType,
                      u8 *Copy_pu8State) {
  if (Copy_pu8State != NULL) {
    *Copy_pu8State =
        HPB_u8ReadLogicalState(Copy_u8Port, Copy_u8Pin, Copy_u8PullType);
  }
}
