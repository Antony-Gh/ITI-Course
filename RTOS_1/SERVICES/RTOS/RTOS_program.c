/*
 * RTOS_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/TIMER/MTIMER_interface.h"

#include "../../CONFIG/RTOS/RTOS_config.h"
#include "../../HW/RTOS/RTOS_private.h"

#include "RTOS_interface.h"

static RTOS_Task_t RTOS_Tasks[RTOS_MAX_TASKS];

static volatile u8 RTOS_u8SchedulerFlag = 0U;

static void RTOS_voidScheduler(void) {
	u8 Local_u8TaskIndex;

	for (Local_u8TaskIndex = 0U; Local_u8TaskIndex < RTOS_MAX_TASKS;
			Local_u8TaskIndex++) {
		if (RTOS_Tasks[Local_u8TaskIndex].State ==
		RTOS_TASK_READY) {
			if (RTOS_Tasks[Local_u8TaskIndex].Counter == 0U) {
				RTOS_Tasks[Local_u8TaskIndex].TaskFunction();

				RTOS_Tasks[Local_u8TaskIndex].Counter =
						RTOS_Tasks[Local_u8TaskIndex].Periodicity;
			}
		}
	}
}

void RTOS_voidInit(void) {
	u8 Local_u8TaskIndex;

	for (Local_u8TaskIndex = 0U; Local_u8TaskIndex < RTOS_MAX_TASKS;
			Local_u8TaskIndex++) {
		RTOS_Tasks[Local_u8TaskIndex].TaskFunction = NULL;
		RTOS_Tasks[Local_u8TaskIndex].Periodicity = 0U;
		RTOS_Tasks[Local_u8TaskIndex].FirstDelay = 0U;
		RTOS_Tasks[Local_u8TaskIndex].Counter = 0U;
		RTOS_Tasks[Local_u8TaskIndex].State = RTOS_TASK_SUSPENDED;
	}

	RTOS_u8SchedulerFlag = 0U;

    MTIMER_voidInit();

    MTIMER_u8SetCompareCallback(MTIMER_TIMER2, RTOS_voidSchedulerTick);
}

u8 RTOS_u8CreateTask(u8 Copy_u8Priority, u32 Copy_u32Periodicity,
		u32 Copy_u32FirstDelay, RTOS_TaskFunction_t Copy_pTaskFunction) {
	if (Copy_u8Priority >= RTOS_MAX_TASKS) {
		return RTOS_ERROR;
	}

	if (Copy_pTaskFunction == NULL) {
		return RTOS_NULL_POINTER;
	}

	if (Copy_u32Periodicity == 0U) {
		return RTOS_ERROR;
	}

	RTOS_Tasks[Copy_u8Priority].TaskFunction = Copy_pTaskFunction;

	RTOS_Tasks[Copy_u8Priority].Periodicity = Copy_u32Periodicity;

	RTOS_Tasks[Copy_u8Priority].FirstDelay = Copy_u32FirstDelay;

	RTOS_Tasks[Copy_u8Priority].Counter = Copy_u32FirstDelay;

	RTOS_Tasks[Copy_u8Priority].State =
	RTOS_TASK_READY;

	return RTOS_OK;
}

u8 RTOS_u8DeleteTask(u8 Copy_u8Priority) {
	if (Copy_u8Priority >= RTOS_MAX_TASKS) {
		return RTOS_ERROR;
	}

	RTOS_Tasks[Copy_u8Priority].TaskFunction = NULL;

	RTOS_Tasks[Copy_u8Priority].Periodicity = 0U;

	RTOS_Tasks[Copy_u8Priority].FirstDelay = 0U;

	RTOS_Tasks[Copy_u8Priority].Counter = 0U;

	RTOS_Tasks[Copy_u8Priority].State = RTOS_TASK_SUSPENDED;

	return RTOS_OK;
}

u8 RTOS_u8SuspendTask(u8 Copy_u8Priority) {
	if (Copy_u8Priority >= RTOS_MAX_TASKS) {
		return RTOS_ERROR;
	}

	RTOS_Tasks[Copy_u8Priority].State = RTOS_TASK_SUSPENDED;

	return RTOS_OK;
}

u8 RTOS_u8ResumeTask(u8 Copy_u8Priority) {
	if (Copy_u8Priority >= RTOS_MAX_TASKS) {
		return RTOS_ERROR;
	}

	if (RTOS_Tasks[Copy_u8Priority].TaskFunction == NULL) {
		return RTOS_ERROR;
	}

	RTOS_Tasks[Copy_u8Priority].State = RTOS_TASK_READY;

	return RTOS_OK;
}

void RTOS_voidStart(void) {
	while (1) {
		if (RTOS_u8SchedulerFlag == 1U) {
			RTOS_u8SchedulerFlag = 0U;
			RTOS_voidScheduler();
		}
	}
}

void RTOS_voidSchedulerTick(void) {
	u8 Local_u8TaskIndex;

	for (Local_u8TaskIndex = 0U; Local_u8TaskIndex < RTOS_MAX_TASKS;
			Local_u8TaskIndex++) {
		if (RTOS_Tasks[Local_u8TaskIndex].State ==
		RTOS_TASK_READY) {
			if (RTOS_Tasks[Local_u8TaskIndex].Counter > 0U) {
				RTOS_Tasks[Local_u8TaskIndex].Counter--;
			}
		}
	}

	RTOS_u8SchedulerFlag = 1U;
}
