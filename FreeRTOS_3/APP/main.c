/*
 * main.c
 *
 *  Created on : Sep 11, 2026
 *      Author : Anthony Gaius
 */

#include "../SERVICES/FreeRTOS/FreeRTOS.h"
#include "../SERVICES/FreeRTOS/semphr.h"
#include "../SERVICES/FreeRTOS/task.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/BIT_MATH.h"
#include "../LIB/DELAY.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"

#define STACK_LED     150U
#define STACK_BUTTON  250U

#define LED1_PERIOD_MS 1000U
#define LED2_PERIOD_MS 3000U
#define LED3_PERIOD_MS 5000U

#define PRIORITY_LED 1U
#define PRIORITY_BUTTON 2U

static volatile u8 g_u8ButtonFlag = 0U;

static TaskHandle_t xLed1Handle = NULL;
static TaskHandle_t xLed2Handle = NULL;
static TaskHandle_t xLed3Handle = NULL;

void Button_ISR(void) {

	DIO_enumTogglePinValue(DIO_PORTA, DIO_PIN0);
	g_u8ButtonFlag = 1U;

	// BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Unblock vTaskButtonHandler from interrupt context
	// xSemaphoreGiveFromISR(xButtonSem, &xHigherPriorityTaskWoken);
	//
	// (void) xHigherPriorityTaskWoken;
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
	(void) xTask;
	(void) pcTaskName;
	DIO_enumTogglePinValue(DIO_PORTA, DIO_PIN1);
	while (1)
		;
}

void vTaskButtonHandler(void *pvParameters) {
	(void) pvParameters;

	static u8 u8Paused = 0U; /* 0 = running, 1 = paused */

	while (1) {
		if (g_u8ButtonFlag != 0U) {
			g_u8ButtonFlag = 0U;

			if (u8Paused == 0U) {
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_LOW);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_LOW);

				vTaskSuspend(xLed1Handle);
				vTaskSuspend(xLed2Handle);
				vTaskSuspend(xLed3Handle);

				u8Paused = 1U;
			} else {
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_HIGH);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_HIGH);
				DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_HIGH);

				vTaskResume(xLed1Handle);
				vTaskResume(xLed2Handle);
				vTaskResume(xLed3Handle);

				u8Paused = 0U;
			}

			vTaskDelay(30);

			// Clear any semaphore given by bounces
			// xSemaphoreTake(xButtonSem, 0);
		}

		vTaskDelay(20);
	}
}

void vTaskLed1(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN5);
		vTaskDelay(LED1_PERIOD_MS);
	}
}

void vTaskLed2(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN6);
		vTaskDelay(LED2_PERIOD_MS);
	}
}

void vTaskLed3(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN7);
		vTaskDelay(LED3_PERIOD_MS);
	}
}

int main(void) {
	BaseType_t xStatus;

	DIO_voidInit();

	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN6, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN7, DIO_OUTPUT);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_HIGH);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_HIGH);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_HIGH);

	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
	DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN0, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTA, DIO_PIN1, DIO_OUTPUT);
	DIO_enumSetPinValue(DIO_PORTA, DIO_PIN0, DIO_LOW);
	DIO_enumSetPinValue(DIO_PORTA, DIO_PIN0, DIO_LOW);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

	//	xButtonSem = xSemaphoreCreateBinary();
	//	if (xButtonSem == NULL) {
	//		while (1)
	//			;
	//	}

	EXTI_voidInit();
	EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
	EXTI_u8SetCallback(EXTI_u8_INT0, Button_ISR);
	EXTI_voidEnableGlobal();

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

	xStatus = xTaskCreate(vTaskButtonHandler, "BTN", STACK_BUTTON, NULL,
	PRIORITY_BUTTON, NULL);
	if (xStatus != pdPASS) {
		while (1)
			;
	}

	vTaskStartScheduler();
	while (1)
		;
	return 0;
}
