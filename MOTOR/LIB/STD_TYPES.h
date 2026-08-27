/*
 * STD_TYPES.h
 *
 *  Created on: Aug 23, 2026
 *      Author: Anthony Gaius
 */

#ifndef STD_TYPES_H_
#define STD_TYPES_H_

/* Unsigned Standard Types */
typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned long int u32;
typedef unsigned long long int u64;

/* Signed Standard Types */
typedef signed char s8;
typedef signed short int s16;
typedef signed long int s32;
typedef signed long long int s64;

/* Floating Point Types */
typedef float f32;
typedef double f64;
typedef long double f128;

/* Pointer Standard Types */
typedef volatile u8 *pu8;
typedef volatile u16 *pu16;
typedef volatile u32 *pu32;
typedef volatile s8 *ps8;
typedef volatile s16 *ps16;
typedef volatile s32 *ps32;

/* Boolean Definitions */
typedef enum { FALSE = 0, TRUE = !FALSE } bool_t;

/* NULL Definition */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* Standard Error Status Enumeration */
typedef enum {
  OK = 0,
  NOK,
  TIME_OUT_ERR,
  BUSY_STATE,
  PARAM_ERR,
  BUFFER_FULL,
  BUFFER_EMPTY
} STD_err_e;

#endif /* STD_TYPES_H_ */
