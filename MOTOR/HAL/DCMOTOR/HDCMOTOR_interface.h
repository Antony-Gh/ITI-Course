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

/* Direction macros */
#define HDCMOTOR_CW   1
#define HDCMOTOR_CCW  2

/* Motor structure */
typedef struct {
    u8 Port;
    u8 Pin1;
    u8 Pin2;
} HDCMOTOR_t;

void HDCMOTOR_voidInit(HDCMOTOR_t* Copy_pstMotor);
void HDCMOTOR_voidRun(HDCMOTOR_t* Copy_pstMotor, u8 Copy_u8Direction);
void HDCMOTOR_voidStop(HDCMOTOR_t* Copy_pstMotor);

#endif /* HDCMOTOR_INTERFACE_H_ */
