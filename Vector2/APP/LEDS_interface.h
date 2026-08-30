/*
 * LEDS_interface.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef LEDS_INTERFACE_H_
#define LEDS_INTERFACE_H_

#include "../LIB/STD_TYPES.h"

void LEDS_voidLeftToRight(u16 Copy_u16DurationMs);
void LEDS_voidRightToLeft(u16 Copy_u16DurationMs);
void LEDS_voidBlinkAll(u16 Copy_u16DurationMs);

extern void (*LEDS_pvAnimationArr[3])(void);

#endif /* LEDS_INTERFACE_H_ */
