/*
 * MEXTI_config.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef MEXTI_CONFIG_H_
#define MEXTI_CONFIG_H_

/* Sense modes */
#define EXTI_FALLING_EDGE 0U
#define EXTI_RISING_EDGE 1U
#define EXTI_LOW_LEVEL 2U
#define EXTI_ANY_CHANGE 3U

/* Enable only the interrupts used by this application */
#define EXTI_INT0_ENABLE 1U
#define EXTI_INT1_ENABLE 0U
#define EXTI_INT2_ENABLE 0U

/* Software ICU starts on a rising edge of the PWM input on INT0 (PD2) */
#define EXTI_INT0_SENSE EXTI_RISING_EDGE
#define EXTI_INT1_SENSE EXTI_FALLING_EDGE
#define EXTI_INT2_SENSE EXTI_FALLING_EDGE

#endif /* MEXTI_CONFIG_H_ */
