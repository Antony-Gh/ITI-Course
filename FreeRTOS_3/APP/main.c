/*
 * main.c
 *
 *  Created on : Sep 11, 2026
 *      Author : Anthony Gaius
 */

/* ─────────────────────── FreeRTOS includes ────────────────────────── */
#include "../SERVICES/FreeRTOS/FreeRTOS.h"
#include "../SERVICES/FreeRTOS/semphr.h"
#include "../SERVICES/FreeRTOS/task.h"

/* ──────────────────────── BSP includes ────────────────────────────── */
#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/BIT_MATH.h"
#include "../LIB/DELAY.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"

/* ─────────────────────────── Defines ──────────────────────────────── */
#define STACK_LED     150U
#define STACK_BUTTON  250U

/* Toggle periods in milliseconds (1 tick = 1 ms) */
#define LED1_PERIOD_MS 1000U /* 1 second  */
#define LED2_PERIOD_MS 3000U /* 3 seconds */
#define LED3_PERIOD_MS 5000U /* 5 seconds */

/* Task priorities */
#define PRIORITY_LED 1U    /* All LED tasks share the same priority */
#define PRIORITY_BUTTON 2U /* Button task pre-empts LED tasks        */

/* Signal flag set by ISR, read by button task */
static volatile u8 g_u8ButtonFlag = 0U;

/* ───────────────────── Global task handles ─────────────────────────── */
static TaskHandle_t xLed1Handle = NULL;
static TaskHandle_t xLed2Handle = NULL;
static TaskHandle_t xLed3Handle = NULL;

/*
 * Binary semaphore used for ISR -> vTaskButtonHandler communication.
 * The ISR gives the semaphore; the handler task takes it and
 * toggles the suspended / running state of all LED tasks.
 */
// static SemaphoreHandle_t xButtonSem = NULL;
/* ─────────────────── EXTI0 ISR  (button on PD2) ───────────────────── */
void Button_ISR(void) {

	DIO_enumTogglePinValue(DIO_PORTA, DIO_PIN0);
	g_u8ButtonFlag = 1U;

	// BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Unblock vTaskButtonHandler from interrupt context */
	//	xSemaphoreGiveFromISR(xButtonSem, &xHigherPriorityTaskWoken);
	//
	//	(void) xHigherPriorityTaskWoken;
	/* Note: In the FreeRTOS AVR port, yielding from an ISR requires the ISR
	 * to be defined with the 'naked' attribute and use specific macros.
	 * Since this function is called from a standard MCAL ISR, we cannot safely
	 * trigger a context switch here. The unblocked task will execute at the
	 * next tick interrupt (up to 1 ms delay), which is fine for a button. */
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	(void) xTask;
	(void) pcTaskName;
	DIO_enumTogglePinValue(DIO_PORTA, DIO_PIN1);
	while (1)
		; /* set a breakpoint here */
}

/* ─────────────────── Button-handler task ───────────────────────────── */
/*
 * Blocks on the binary semaphore indefinitely.
 * Each successful Take toggles between "system paused" and "system running"
 * by calling vTaskSuspend / vTaskResume on every LED task.
 */
void vTaskButtonHandler(void *pvParameters) {
	(void) pvParameters;

	static u8 u8Paused = 0U; /* 0 = running, 1 = paused */

	while (1) {
		/* Wait for a button press */
		if (g_u8ButtonFlag != 0U) {
            g_u8ButtonFlag = 0U;

			if (u8Paused == 0U) {
				/* Force LEDs OFF when paused (DIO_LOW turns them OFF based on user feedback) */
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_LOW);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_LOW);

				/* ── Pause all LED tasks ── */
				vTaskSuspend(xLed1Handle);
				vTaskSuspend(xLed2Handle);
				vTaskSuspend(xLed3Handle);

				u8Paused = 1U;
			} else {
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_HIGH);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_HIGH);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_HIGH);

				/* ── Resume all LED tasks ── */
				vTaskResume(xLed1Handle);
				vTaskResume(xLed2Handle);
				vTaskResume(xLed3Handle);

				u8Paused = 0U;
			}

			/* Wait a little longer for any release-bounce to settle */
			vTaskDelay(30);

			/* Clear any semaphore given by bounces */
			// xSemaphoreTake(xButtonSem, 0);
		}

		 vTaskDelay(20);
	}
}

/* ─────────────────── LED Task 1 – toggles every 1 s ───────────────── */
void vTaskLed1(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN5);
		/* Use vTaskDelay to avoid catch-up spinning when resumed from suspension */
		vTaskDelay(LED1_PERIOD_MS);
	}
}

/* ─────────────────── LED Task 2 – toggles every 3 s ───────────────── */
void vTaskLed2(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN6);
		vTaskDelay(LED2_PERIOD_MS);
	}
}

/* ─────────────────── LED Task 3 – toggles every 5 s ───────────────── */
void vTaskLed3(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN7);
		vTaskDelay(LED3_PERIOD_MS);
	}
}

/* ──────────────────────────── main ────────────────────────────────── */
int main(void) {
	BaseType_t xStatus;

	/* ── GPIO init ── */
	DIO_voidInit();

	/* LED output pins – start HIGH (OFF for active-low LEDs) */
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN6, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN7, DIO_OUTPUT);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_HIGH);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_HIGH);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_HIGH);

	/* Button: input with internal pull-up (active-low) */
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
	DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN0, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN1, DIO_OUTPUT);
	DIO_enumSetPinValue(DIO_PORTA, DIO_PIN0, DIO_LOW);
	DIO_enumSetPinValue(DIO_PORTA, DIO_PIN0, DIO_LOW);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

	/* ── Create binary semaphore for ISR -> button task signaling ── */
	//	xButtonSem = xSemaphoreCreateBinary();
	//	if (xButtonSem == NULL) {
	//		while (1)
	//			;
	//	}

	/* ── EXTI init ── */
	EXTI_voidInit();
	EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
	EXTI_u8SetCallback(EXTI_u8_INT0, Button_ISR);
	EXTI_voidEnableGlobal();

	/* ── Create LED tasks (store handles for suspend/resume) ── */
	xStatus = xTaskCreate(vTaskLed1, "LED1", STACK_LED, NULL, PRIORITY_LED,
			&xLed1Handle);
	if (xStatus != pdPASS) {
		while (1)
			;
	}

	xStatus = xTaskCreate(vTaskLed2, "LED2", STACK_LED, NULL, PRIORITY_LED,
			&xLed2Handle);
	if (xStatus != pdPASS) {
		while (1)
			;
	}

	xStatus = xTaskCreate(vTaskLed3, "LED3", STACK_LED, NULL, PRIORITY_LED,
			&xLed3Handle);
	if (xStatus != pdPASS) {
		while (1)
			;
	}

	/* ── Create button-handler task (higher priority so it pre-empts LEDs) ── */
	xStatus = xTaskCreate(vTaskButtonHandler, "BTN", STACK_BUTTON, NULL,
	PRIORITY_BUTTON, NULL);
	if (xStatus != pdPASS) {
		while (1)
			;
	}

	/* ── Start the scheduler – never returns ── */
	vTaskStartScheduler();
	while (1)
		;
	return 0;
}
