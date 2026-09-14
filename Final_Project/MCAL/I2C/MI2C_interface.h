/*
 * MI2C_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef MCAL_I2C_MI2C_INTERFACE_H_
#define MCAL_I2C_MI2C_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define MI2C_OK                 0U
#define MI2C_ERROR              1U
#define MI2C_START_ERROR        2U
#define MI2C_ADDRESS_ERROR      3U
#define MI2C_DATA_ERROR         4U
#define MI2C_READ_ERROR         5U

void MI2C_voidInit(void);

u8 MI2C_u8Start(void);

u8 MI2C_u8SendAddress(u8 Copy_u8Address);

u8 MI2C_u8WriteData(u8 Copy_u8Data);

u8 MI2C_u8ReadDataWithACK(u8 *Copy_pu8Data);

u8 MI2C_u8ReadDataWithNACK(u8 *Copy_pu8Data);

void MI2C_voidStop(void);

#endif /* MCAL_I2C_MI2C_INTERFACE_H_ */
