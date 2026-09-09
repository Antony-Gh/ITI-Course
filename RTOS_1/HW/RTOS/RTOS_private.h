/*
 * RTOS_private.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef HW_RTOS_RTOS_PRIVATE_H_
#define HW_RTOS_RTOS_PRIVATE_H_

typedef struct {
	void (*TaskFunction)(void);
	u32 Periodicity;
	u32 FirstDelay;
	u32 Counter;
	u8 State;
} RTOS_Task_t;

static void RTOS_voidScheduler(void);

#endif /* HW_RTOS_RTOS_PRIVATE_H_ */
