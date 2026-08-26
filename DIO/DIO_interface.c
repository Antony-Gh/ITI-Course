/*
 * DIO_interface.c
 *
 *  Created on: Aug 24, 2026
 *      Author: anthonygaius
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "DIO_interface.h"
#include "DIO_config.h"
#include "DIO_private.h"

void DIO_voidInit(void)
{
    /* Initialize all Port Directions */
    DDRA = DIO_PORTA_DIR;
    DDRB = DIO_PORTB_DIR;
    DDRC = DIO_PORTC_DIR;
    DDRD = DIO_PORTD_DIR;

    /* Initialize all Port Values */
    PORTA = DIO_PORTA_VAL;
    PORTB = DIO_PORTB_VAL;
    PORTC = DIO_PORTC_VAL;
    PORTD = DIO_PORTD_VAL;
}

DIO_ErrorStatus_t DIO_enumSetPinDirection(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Direction)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;

    if (Copy_u8Pin <= DIO_PIN7)
    {
        if (Copy_u8Direction == DIO_OUTPUT)
        {
            switch (Copy_u8Port)
            {
                case DIO_PORTA: SET_BIT(DDRA, Copy_u8Pin); break;
                case DIO_PORTB: SET_BIT(DDRB, Copy_u8Pin); break;
                case DIO_PORTC: SET_BIT(DDRC, Copy_u8Pin); break;
                case DIO_PORTD: SET_BIT(DDRD, Copy_u8Pin); break;
                default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
            }
        }
        else if (Copy_u8Direction == DIO_INPUT)
        {
            switch (Copy_u8Port)
            {
                case DIO_PORTA: CLR_BIT(DDRA, Copy_u8Pin); break;
                case DIO_PORTB: CLR_BIT(DDRB, Copy_u8Pin); break;
                case DIO_PORTC: CLR_BIT(DDRC, Copy_u8Pin); break;
                case DIO_PORTD: CLR_BIT(DDRD, Copy_u8Pin); break;
                default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
            }
        }
        else
        {
            Local_enumErrorState = DIO_ERR_INVALID_DIRECTION;
        }
    }
    else
    {
        Local_enumErrorState = DIO_ERR_INVALID_PIN;
    }
    return Local_enumErrorState;
}

DIO_ErrorStatus_t DIO_enumSetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8Value)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;

    if (Copy_u8Pin <= DIO_PIN7)
    {
        if (Copy_u8Value == DIO_HIGH)
        {
            switch (Copy_u8Port)
            {
                case DIO_PORTA: SET_BIT(PORTA, Copy_u8Pin); break;
                case DIO_PORTB: SET_BIT(PORTB, Copy_u8Pin); break;
                case DIO_PORTC: SET_BIT(PORTC, Copy_u8Pin); break;
                case DIO_PORTD: SET_BIT(PORTD, Copy_u8Pin); break;
                default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
            }
        }
        else if (Copy_u8Value == DIO_LOW)
        {
            switch (Copy_u8Port)
            {
                case DIO_PORTA: CLR_BIT(PORTA, Copy_u8Pin); break;
                case DIO_PORTB: CLR_BIT(PORTB, Copy_u8Pin); break;
                case DIO_PORTC: CLR_BIT(PORTC, Copy_u8Pin); break;
                case DIO_PORTD: CLR_BIT(PORTD, Copy_u8Pin); break;
                default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
            }
        }
        else
        {
            Local_enumErrorState = DIO_ERR_INVALID_VALUE;
        }
    }
    else
    {
        Local_enumErrorState = DIO_ERR_INVALID_PIN;
    }
    return Local_enumErrorState;
}

DIO_ErrorStatus_t DIO_enumGetPinValue(u8 Copy_u8Port, u8 Copy_u8Pin, u8* Copy_pu8Value)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;
    
    if (Copy_pu8Value == NULL)
    {
        Local_enumErrorState = DIO_ERR_NULL_POINTER;
    }
    else if (Copy_u8Pin > DIO_PIN7)
    {
        Local_enumErrorState = DIO_ERR_INVALID_PIN;
    }
    else
    {
        switch (Copy_u8Port)
        {
            case DIO_PORTA: *Copy_pu8Value = GET_BIT(PINA, Copy_u8Pin); break;
            case DIO_PORTB: *Copy_pu8Value = GET_BIT(PINB, Copy_u8Pin); break;
            case DIO_PORTC: *Copy_pu8Value = GET_BIT(PINC, Copy_u8Pin); break;
            case DIO_PORTD: *Copy_pu8Value = GET_BIT(PIND, Copy_u8Pin); break;
            default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
        }
    }
    return Local_enumErrorState;
}

DIO_ErrorStatus_t DIO_enumSetPortDirection(u8 Copy_u8Port, u8 Copy_u8Direction)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;
    switch (Copy_u8Port)
    {
        case DIO_PORTA: DDRA = Copy_u8Direction; break;
        case DIO_PORTB: DDRB = Copy_u8Direction; break;
        case DIO_PORTC: DDRC = Copy_u8Direction; break;
        case DIO_PORTD: DDRD = Copy_u8Direction; break;
        default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
    }
    return Local_enumErrorState;
}

DIO_ErrorStatus_t DIO_enumSetPortValue(u8 Copy_u8Port, u8 Copy_u8Value)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;
    switch (Copy_u8Port)
    {
        case DIO_PORTA: PORTA = Copy_u8Value; break;
        case DIO_PORTB: PORTB = Copy_u8Value; break;
        case DIO_PORTC: PORTC = Copy_u8Value; break;
        case DIO_PORTD: PORTD = Copy_u8Value; break;
        default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
    }
    return Local_enumErrorState;
}

DIO_ErrorStatus_t DIO_enumGetPortValue(u8 Copy_u8Port, u8* Copy_pu8Value)
{
    DIO_ErrorStatus_t Local_enumErrorState = DIO_OK;
    if (Copy_pu8Value == NULL)
    {
        Local_enumErrorState = DIO_ERR_NULL_POINTER;
    }
    else
    {
        switch (Copy_u8Port)
        {
            case DIO_PORTA: *Copy_pu8Value = PINA; break;
            case DIO_PORTB: *Copy_pu8Value = PINB; break;
            case DIO_PORTC: *Copy_pu8Value = PINC; break;
            case DIO_PORTD: *Copy_pu8Value = PIND; break;
            default: Local_enumErrorState = DIO_ERR_INVALID_PORT; break;
        }
    }
    return Local_enumErrorState;
}
