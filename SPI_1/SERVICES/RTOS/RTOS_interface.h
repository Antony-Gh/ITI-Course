/*
 * RTOS_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef SERVICES_RTOS_RTOS_INTERFACE_H_
#define SERVICES_RTOS_RTOS_INTERFACE_H_

#define RTOS_OK                 0U
#define RTOS_ERROR              1U
#define RTOS_NULL_POINTER       2U
#define RTOS_NO_AVAILABLE_TASK  3U

typedef void (*RTOS_TaskFunction_t)(void);

void RTOS_voidInit(void);

u8 RTOS_u8CreateTask(u8 Copy_u8Priority, u32 Copy_u32Periodicity,
		u32 Copy_u32FirstDelay, RTOS_TaskFunction_t Copy_pTaskFunction);

u8 RTOS_u8DeleteTask(u8 Copy_u8Priority);

u8 RTOS_u8SuspendTask(u8 Copy_u8Priority);

u8 RTOS_u8ResumeTask(u8 Copy_u8Priority);

void RTOS_voidStart(void);

void RTOS_voidSchedulerTick(void);

#endif /* SERVICES_RTOS_RTOS_INTERFACE_H_ */
