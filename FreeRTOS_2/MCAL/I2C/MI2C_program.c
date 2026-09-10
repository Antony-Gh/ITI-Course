/*
 * MI2C_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */


#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../LIB/REGISTERS.h"

#include "../../CONFIG/I2C/MI2C_config.h"
#include "../../HW/I2C/MI2C_private.h"
#include "MI2C_interface.h"

void MI2C_voidInit(void)
{
    u8 Local_u8TWBR;

    TWSR = 0x00;

    Local_u8TWBR =
        (u8)(((MI2C_CPU_FREQUENCY / MI2C_SCL_FREQUENCY) - 16UL) / 2UL);

    TWBR = Local_u8TWBR;

    TWAR = 0x00;

    TWCR = (1U << TWEN);
}

u8 MI2C_u8Start(void)
{
    TWCR = (1U << TWINT) |
           (1U << TWSTA) |
           (1U << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0U);

    if((TWSR & MI2C_STATUS_MASK) == MI2C_START_TRANSMITTED ||
       (TWSR & MI2C_STATUS_MASK) == MI2C_REP_START_TRANSMITTED)
    {
        return MI2C_OK;
    }

    return MI2C_START_ERROR;
}

u8 MI2C_u8SendAddress(u8 Copy_u8Address)
{
    TWDR = Copy_u8Address;

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0U);

    if((TWSR & MI2C_STATUS_MASK) == MI2C_SLA_W_ACK ||
       (TWSR & MI2C_STATUS_MASK) == MI2C_SLA_R_ACK)
    {
        return MI2C_OK;
    }

    return MI2C_ADDRESS_ERROR;
}

u8 MI2C_u8WriteData(u8 Copy_u8Data)
{
    TWDR = Copy_u8Data;

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0U);

    if((TWSR & MI2C_STATUS_MASK) == MI2C_DATA_W_ACK)
    {
        return MI2C_OK;
    }

    return MI2C_DATA_ERROR;
}

u8 MI2C_u8ReadDataWithACK(u8 *Copy_pu8Data)
{
    if(Copy_pu8Data == NULL)
    {
        return MI2C_ERROR;
    }

    TWCR = (1U << TWINT) |
           (1U << TWEN) |
           (1U << TWEA);

    while(GET_BIT(TWCR, TWINT) == 0U);

    if((TWSR & MI2C_STATUS_MASK) == MI2C_DATA_R_ACK)
    {
        *Copy_pu8Data = TWDR;
        return MI2C_OK;
    }

    return MI2C_READ_ERROR;
}

u8 MI2C_u8ReadDataWithNACK(u8 *Copy_pu8Data)
{
    if(Copy_pu8Data == NULL)
    {
        return MI2C_ERROR;
    }

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0U);

    if((TWSR & MI2C_STATUS_MASK) == MI2C_DATA_R_NACK)
    {
        *Copy_pu8Data = TWDR;
        return MI2C_OK;
    }

    return MI2C_READ_ERROR;
}

void MI2C_voidStop(void)
{
    TWCR = (1U << TWINT) |
           (1U << TWSTO) |
           (1U << TWEN);
}
