/*
 * HDCMOTOR_interface.h
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#ifndef HDCMOTOR_INTERFACE_H_
#define HDCMOTOR_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/DCMOTOR/HDCMOTOR_config.h"
#include "../../HW/DCMOTOR/HDCMOTOR_private.h"

#define HDCMOTOR_STOP 0U
#define HDCMOTOR_CW 1U
#define HDCMOTOR_CCW 2U

typedef enum {
  HDCMOTOR_STATE_STOPPED = 0,
  HDCMOTOR_STATE_SWITCHING,
  HDCMOTOR_STATE_RUNNING
} HDCMOTOR_State_e;

typedef struct {
  u8 Port;
  u8 RelayCwPin;
  u8 RelayCcwPin;
  u8 ActiveDirection;
  u8 RequestedDirection;
  HDCMOTOR_State_e State;
  u32 SwitchDeadlineMs;
} HDCMOTOR_t;

STD_err_e HDCMOTOR_enumInit(HDCMOTOR_t *Copy_pstMotor);
STD_err_e HDCMOTOR_enumRequestRun(HDCMOTOR_t *Copy_pstMotor,
                                  u8 Copy_u8Direction);
STD_err_e HDCMOTOR_enumRequestStop(HDCMOTOR_t *Copy_pstMotor);
void HDCMOTOR_voidUpdate(HDCMOTOR_t *Copy_pstMotor);
STD_err_e HDCMOTOR_enumGetDirection(const HDCMOTOR_t *Copy_pstMotor,
                                    u8 *Copy_pu8Direction);
HDCMOTOR_State_e HDCMOTOR_enumGetState(const HDCMOTOR_t *Copy_pstMotor);

/* Legacy blocking API */
void HDCMOTOR_voidInit(HDCMOTOR_t *Copy_pstMotor);
void HDCMOTOR_voidRun(HDCMOTOR_t *Copy_pstMotor, u8 Copy_u8Direction);
void HDCMOTOR_voidStop(HDCMOTOR_t *Copy_pstMotor);

#endif /* HDCMOTOR_INTERFACE_H_ */
