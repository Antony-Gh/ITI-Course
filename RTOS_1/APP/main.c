/*
 * main.c
 *
 *  Created on: Aug 31, 2026
 *      Author: Anthony Gaius
 */


#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO/MDIO_interface.h"

#include "../HAL/LED/HLED_interface.h"
#include "../SERVICES/RTOS/RTOS_interface.h"


void LED_Task(void)
{
	HLED_voidToggleLed(0);
}


void LED2_Task(void)
{
	HLED_voidToggleLed(1);
}


void LED3_Task(void)
{
	HLED_voidToggleLed(2);
}


int main(void)
{
	DIO_voidInit();

    HLED_voidInit();

    RTOS_voidInit();

    RTOS_u8CreateTask(
        0,
        500,
        0,
        LED_Task
    );

    RTOS_u8CreateTask(
        1,
        1000,
        0,
        LED2_Task
    );

    RTOS_u8CreateTask(
        2,
        2000,
        0,
        LED3_Task
    );

    RTOS_voidStart();

    return 0;
}
