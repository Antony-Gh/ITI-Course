/*
 * KPD_I2C_program.c
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Standalone Keypad HAL driver — I2C mode using TWO PCF8574 modules
 */

#include "../../CONFIG/KPD_I2C/KPD_I2C_config.h"
#include "../../HW/KPD_I2C/KPD_I2C_private.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../LIB/DELAY.h"
#include "../../MCAL/I2C/MI2C_interface.h"
#include "KPD_I2C_interface.h"

static const u8 Local_u8KPDArr[KPD_I2C_NUM_ROWS][KPD_I2C_NUM_COLS] = KPD_I2C_KEYS;

static void KPD_I2C_voidSendI2CByte(u8 Copy_u8Address, u8 Copy_u8Data)
{
    MI2C_u8Start();
    MI2C_u8SendAddress(Copy_u8Address << 1); 
    MI2C_u8WriteData(Copy_u8Data);
    MI2C_voidStop();
}

static u8 KPD_I2C_u8ReadI2CByte(u8 Copy_u8Address)
{
    u8 local_u8Data = 0xFF;
    MI2C_u8Start();
    MI2C_u8SendAddress((Copy_u8Address << 1) | 0x01U);
    MI2C_u8ReadDataWithNACK(&local_u8Data);
    MI2C_voidStop();
    return local_u8Data;
}

void KPD_I2C_voidInit(void)
{
    KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, 0xFF);
    KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_COLS_ADDR, 0xFF);
}

u8 KPD_I2C_u8ScanKey(void)
{
    u8 Local_u8RowIdx;
    u8 Local_u8ColIdx;
    u8 Local_u8ColsState;

    for (Local_u8RowIdx = 0; Local_u8RowIdx < KPD_I2C_NUM_ROWS; Local_u8RowIdx++) {
        u8 local_u8RowMask = ~(1U << Local_u8RowIdx);
        KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, local_u8RowMask);
        DELAY_voidMs(1);

        Local_u8ColsState = KPD_I2C_u8ReadI2CByte(KPD_I2C_PCF_COLS_ADDR);

        if (Local_u8ColsState != 0xFF) {
            for (Local_u8ColIdx = 0; Local_u8ColIdx < KPD_I2C_NUM_COLS; Local_u8ColIdx++) {
                if (GET_BIT(Local_u8ColsState, Local_u8ColIdx) == 0U) {
                    KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, 0xFF);
                    return Local_u8KPDArr[Local_u8RowIdx][Local_u8ColIdx];
                }
            }
        }
    }

    KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, 0xFF);
    return KPD_I2C_NOT_PRESSED;
}

u8 KPD_I2C_u8GetPressedKey(void)
{
    u8 Local_u8PressedKey = KPD_I2C_NOT_PRESSED;
    u8 Local_u8ColsState;

    Local_u8PressedKey = KPD_I2C_u8ScanKey();

    if (Local_u8PressedKey != KPD_I2C_NOT_PRESSED) {
        do {
            KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, 0x00);
            Local_u8ColsState = KPD_I2C_u8ReadI2CByte(KPD_I2C_PCF_COLS_ADDR);
            
            if(Local_u8ColsState != 0xFF) {
                DELAY_voidMs(10);
            }
        } while (Local_u8ColsState != 0xFF);

        KPD_I2C_voidSendI2CByte(KPD_I2C_PCF_ROWS_ADDR, 0xFF);
        DELAY_voidMs(20);
        return Local_u8PressedKey;
    }

    return KPD_I2C_NOT_PRESSED;
}
