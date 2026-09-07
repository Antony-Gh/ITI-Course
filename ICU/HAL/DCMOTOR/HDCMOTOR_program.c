/*
 * HDCMOTOR_program.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/TIMER/MTIMER_interface.h"

#include "HDCMOTOR_interface.h"

static void HDCMOTOR_voidSetRelayPins(u8 Copy_u8Port, u8 Copy_u8CwPin,
                                      u8 Copy_u8CcwPin, u8 Copy_u8CwState,
                                      u8 Copy_u8CcwState) {
  if (Copy_u8CwState == HDCMOTOR_RELAY_ACTIVE_LEVEL) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin, HDCMOTOR_RELAY_ACTIVE_LEVEL);
  } else if (Copy_u8CcwState == HDCMOTOR_RELAY_ACTIVE_LEVEL) {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_ACTIVE_LEVEL);
  } else {
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
    DIO_enumSetPinValue(Copy_u8Port, Copy_u8CcwPin,
                        HDCMOTOR_RELAY_INACTIVE_LEVEL);
  }
}

static void HDCMOTOR_voidDeenergizeRelays(HDCMOTOR_t *Copy_pstMotor) {
  HDCMOTOR_voidSetRelayPins(Copy_pstMotor->Port, Copy_pstMotor->RelayCwPin,
                            Copy_pstMotor->RelayCcwPin,
                            HDCMOTOR_RELAY_INACTIVE_LEVEL,
                            HDCMOTOR_RELAY_INACTIVE_LEVEL);
}

static void HDCMOTOR_voidEnergizeDirection(HDCMOTOR_t *Copy_pstMotor,
                                           u8 Copy_u8Direction) {
  if (Copy_u8Direction == HDCMOTOR_CW) {
    HDCMOTOR_voidSetRelayPins(Copy_pstMotor->Port, Copy_pstMotor->RelayCwPin,
                              Copy_pstMotor->RelayCcwPin,
                              HDCMOTOR_RELAY_ACTIVE_LEVEL,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL);
  } else if (Copy_u8Direction == HDCMOTOR_CCW) {
    HDCMOTOR_voidSetRelayPins(Copy_pstMotor->Port, Copy_pstMotor->RelayCwPin,
                              Copy_pstMotor->RelayCcwPin,
                              HDCMOTOR_RELAY_INACTIVE_LEVEL,
                              HDCMOTOR_RELAY_ACTIVE_LEVEL);
  }
}

STD_err_e HDCMOTOR_enumInit(HDCMOTOR_t *Copy_pstMotor) {
  if (Copy_pstMotor == NULL) {
    return PARAM_ERR;
  }

  Copy_pstMotor->ActiveDirection = HDCMOTOR_STOP;
  Copy_pstMotor->RequestedDirection = HDCMOTOR_STOP;
  Copy_pstMotor->State = HDCMOTOR_STATE_STOPPED;
  Copy_pstMotor->SwitchDeadlineMs = 0U;

  DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->RelayCwPin,
                          DIO_OUTPUT);
  DIO_enumSetPinDirection(Copy_pstMotor->Port, Copy_pstMotor->RelayCcwPin,
                          DIO_OUTPUT);
  HDCMOTOR_voidDeenergizeRelays(Copy_pstMotor);

  return OK;
}

STD_err_e HDCMOTOR_enumRequestRun(HDCMOTOR_t *Copy_pstMotor,
                                  u8 Copy_u8Direction) {
  u32 local_u32Now;

  if (Copy_pstMotor == NULL) {
    return PARAM_ERR;
  }

  if (Copy_u8Direction != HDCMOTOR_CW &&
      Copy_u8Direction != HDCMOTOR_CCW) {
    return PARAM_ERR;
  }

  if (Copy_pstMotor->State == HDCMOTOR_STATE_SWITCHING) {
    return BUSY_STATE;
  }

  if (Copy_pstMotor->State == HDCMOTOR_STATE_RUNNING &&
      Copy_pstMotor->ActiveDirection == Copy_u8Direction) {
    return OK;
  }

  local_u32Now = MTIMER_u32GetMillis();
  Copy_pstMotor->RequestedDirection = Copy_u8Direction;
  HDCMOTOR_voidDeenergizeRelays(Copy_pstMotor);

  if (Copy_pstMotor->ActiveDirection != HDCMOTOR_STOP) {
    Copy_pstMotor->SwitchDeadlineMs =
        local_u32Now + HDCMOTOR_RELAY_SWITCH_DELAY_MS;
  } else {
    Copy_pstMotor->SwitchDeadlineMs = local_u32Now;
  }

  Copy_pstMotor->State = HDCMOTOR_STATE_SWITCHING;
  return OK;
}

STD_err_e HDCMOTOR_enumRequestStop(HDCMOTOR_t *Copy_pstMotor) {
  if (Copy_pstMotor == NULL) {
    return PARAM_ERR;
  }

  HDCMOTOR_voidDeenergizeRelays(Copy_pstMotor);
  Copy_pstMotor->ActiveDirection = HDCMOTOR_STOP;
  Copy_pstMotor->RequestedDirection = HDCMOTOR_STOP;
  Copy_pstMotor->State = HDCMOTOR_STATE_STOPPED;
  Copy_pstMotor->SwitchDeadlineMs = 0U;

  return OK;
}

void HDCMOTOR_voidUpdate(HDCMOTOR_t *Copy_pstMotor) {
  if (Copy_pstMotor == NULL) {
    return;
  }

  if (Copy_pstMotor->State != HDCMOTOR_STATE_SWITCHING) {
    return;
  }

  if (MTIMER_u32GetMillis() >= Copy_pstMotor->SwitchDeadlineMs) {
    HDCMOTOR_voidEnergizeDirection(Copy_pstMotor,
                                   Copy_pstMotor->RequestedDirection);
    Copy_pstMotor->ActiveDirection = Copy_pstMotor->RequestedDirection;
    Copy_pstMotor->State = HDCMOTOR_STATE_RUNNING;
  }
}

STD_err_e HDCMOTOR_enumGetDirection(const HDCMOTOR_t *Copy_pstMotor,
                                    u8 *Copy_pu8Direction) {
  if (Copy_pstMotor == NULL || Copy_pu8Direction == NULL) {
    return PARAM_ERR;
  }

  *Copy_pu8Direction = Copy_pstMotor->ActiveDirection;
  return OK;
}

HDCMOTOR_State_e HDCMOTOR_enumGetState(const HDCMOTOR_t *Copy_pstMotor) {
  if (Copy_pstMotor == NULL) {
    return HDCMOTOR_STATE_STOPPED;
  }

  return Copy_pstMotor->State;
}

void HDCMOTOR_voidInit(HDCMOTOR_t *Copy_pstMotor) {
  (void)HDCMOTOR_enumInit(Copy_pstMotor);
}

void HDCMOTOR_voidRun(HDCMOTOR_t *Copy_pstMotor, u8 Copy_u8Direction) {
  if (HDCMOTOR_enumRequestRun(Copy_pstMotor, Copy_u8Direction) == OK) {
    while (Copy_pstMotor != NULL &&
           Copy_pstMotor->State == HDCMOTOR_STATE_SWITCHING) {
      HDCMOTOR_voidUpdate(Copy_pstMotor);
    }
  }
}

void HDCMOTOR_voidStop(HDCMOTOR_t *Copy_pstMotor) {
  (void)HDCMOTOR_enumRequestStop(Copy_pstMotor);
}
