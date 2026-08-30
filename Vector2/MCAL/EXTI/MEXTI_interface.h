/*
 * MEXTI_interface.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef MEXTI_INTERFACE_H_
#define MEXTI_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define EXTI_u8_INT0 0U
#define EXTI_u8_INT1 1U
#define EXTI_u8_INT2 2U

void EXTI_voidInit(void);
u8 EXTI_u8SetCallback(u8 Copy_u8IntId, void (*Copy_pvCallback)(void));

#endif /* MEXTI_INTERFACE_H_ */
