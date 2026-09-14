/*
 * MI2C_config.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef CONFIG_I2C_MI2C_CONFIG_H_
#define CONFIG_I2C_MI2C_CONFIG_H_

#include "../../LIB/STD_TYPES.h"

#define MI2C_CPU_FREQUENCY       F_CPU
#define MI2C_SCL_FREQUENCY       100000UL

/* Prescaler Value: it's just a variable to make it
   easier to do the calculations */
#define MI2C_PRESCALER_VALUE     1U

/* 00 -> 1
   01 -> 4
   10 -> 16
   11 -> 64 */
#define MI2C_PRESCALER_BITS      0U

#endif /* CONFIG_I2C_MI2C_CONFIG_H_ */
