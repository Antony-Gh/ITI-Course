/*
 * DIO_interface.h
 *
 *  Created on: Aug 24, 2026
 *      Author: Anthony Gaius
 */

#ifndef DIO_INTERFACE_H_
#define DIO_INTERFACE_H_

#include "STD_TYPES.h"

/* Macros for Ports */
#define DIO_PORTA  0
#define DIO_PORTB  1
#define DIO_PORTC  2
#define DIO_PORTD  3

/* Macros for Pins */
#define DIO_PIN0   0
#define DIO_PIN1   1
#define DIO_PIN2   2
#define DIO_PIN3   3
#define DIO_PIN4   4
#define DIO_PIN5   5
#define DIO_PIN6   6
#define DIO_PIN7   7

/* Macros for Pin Direction */
#define DIO_INPUT  0
#define DIO_OUTPUT 1

/* Macros for Pin Value */
#define DIO_LOW    0
#define DIO_HIGH   1

/* Macros for Port Direction */
#define DIO_PORT_INPUT  0x00
#define DIO_PORT_OUTPUT 0xFF

/* Macros for Port Value */
#define DIO_PORT_LOW    0x00
#define DIO_PORT_HIGH   0xFF

/* Error Codes Enum */
typedef enum {
    DIO_OK = 0,
    DIO_ERR_INVALID_PORT,
    DIO_ERR_INVALID_PIN,
    DIO_ERR_INVALID_DIRECTION,
    DIO_ERR_INVALID_VALUE,
    DIO_ERR_NULL_POINTER
} DIO_ErrorStatus_t;

void DIO_voidInit (void);

DIO_ErrorStatus_t DIO_enumSetPinDirection(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Direction);
DIO_ErrorStatus_t DIO_enumSetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Value);
DIO_ErrorStatus_t DIO_enumGetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8* Copy_pu8Value);
DIO_ErrorStatus_t DIO_enumSetPortDirection(u8 Copy_u8Port, u8 Copy_u8Direction);
DIO_ErrorStatus_t DIO_enumSetPortValue(u8 Copy_u8Port, u8 Copy_u8Value);
DIO_ErrorStatus_t DIO_enumGetPortValue(u8 Copy_u8Port, u8* Copy_pu8Value);

#endif /* DIO_INTERFACE_H_ */
