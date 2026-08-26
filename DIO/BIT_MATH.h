/*
 * BIT_MATH.h
 *
 *  Created on: Aug 23, 2026
 *      Author: Anthony Gaius
 */

#ifndef BIT_MATH_H_
#define BIT_MATH_H_

#include "STD_TYPES.h"


/* Basic Bit Operations */
/* Set a specific bit to 1 */
#define SET_BIT(REG, BIT)        ((REG) |= (1U << (BIT)))

/* Clear a specific bit to 0 */
#define CLR_BIT(REG, BIT)        ((REG) &= ~(1U << (BIT)))

/* Toggle a specific bit */
#define TOG_BIT(REG, BIT)        ((REG) ^= (1U << (BIT)))

/* Get/read the value of a specific bit */
#define GET_BIT(REG, BIT)        (((REG) >> (BIT)) & 1U)


/* Advanced Bit Operations */
/* Assign a value to a specific bit */
#define ASSIGN_BIT(REG, BIT, VALUE) ((VALUE) ? SET_BIT((REG), (BIT)) : CLR_BIT((REG), (BIT)))

/* Write a value to a specific bit */
#define WRITE_BIT(REG, BIT, VALUE)  ((REG) = (((REG) & ~(1UL << (BIT))) | ((VALUE) << (BIT))))

/* Rotate a register right by a specific number of bits */
#define ROTATE_RIGHT(REG, BIT)    ((REG) = ((REG) >> (BIT)) | ((REG) << ((sizeof(REG) * 8) - (BIT))))

/* Rotate a register left by a specific number of bits */
#define ROTATE_LEFT(REG, BIT)     ((REG) = ((REG) << (BIT)) | ((REG) >> ((sizeof(REG) * 8) - (BIT))))


/* Multi-Bit Masking Operations */
/* Set all bits of a register to 1 */
#define SET_REG(REG)                ((REG) = 0xFF)

/* Clear all bits of a register to 0 */
#define CLR_REG(REG)                ((REG) = 0x00)

/* Toggle all bits of a register */
#define TOG_REG(REG)                ((REG) ^= 0xFF)

/* Write a value to a register */
#define WRITE_REG(REG, VAL)         ((REG) = (VAL))

#endif /* BIT_MATH_H_ */
