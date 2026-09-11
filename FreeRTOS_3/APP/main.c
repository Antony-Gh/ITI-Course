/*
 * main.c  –  FreeRTOS Lab_1
 *
 *
 *  Created on : Sep 10, 2026
 *      Author : Anthony Gaius
 */

#include "../SERVICES/FreeRTOS/FreeRTOS.h"
#include "../SERVICES/FreeRTOS/task.h"
#include "../SERVICES/FreeRTOS/semphr.h"
#include "../SERVICES/FreeRTOS/queue.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../LIB/DELAY.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"
#include "../HAL/LCD/HLCD_interface.h"

#define TASK_STACK_INITIAL   150U
#define TASK_STACK_LARGER    400U

static SemaphoreHandle_t xCounting_Semaphore = NULL;

void Button_ISR(void) {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	xSemaphoreGiveFromISR(xCounting_Semaphore, &xHigherPriorityTaskWoken);
	(void) xHigherPriorityTaskWoken;
}

void vApplicationTickHook(void) {
	static uint16_t s_u16Count = 0;
	if (++s_u16Count >= 1000U) {
		s_u16Count = 0;
		PORTB ^= (1u << 0);
	}
}

void vTaskLed1(void *pvParameters) {
	(void) pvParameters;
	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN5);
		vTaskDelay(500);
	}
}

void vTaskLed2(void *pvParameters) {
	(void) pvParameters;
	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN6);
		vTaskDelay(1000);
	}
}

void vTaskLed3(void *pvParameters) {
	(void) pvParameters;
	while (1) {
		DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN7);
		vTaskDelay(2000);
	}
}

void vTaskLCD1(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		if (xSemaphoreTake(xCounting_Semaphore, portMAX_DELAY) == pdTRUE) {
			HLCD_voidClearScreen();
			HLCD_voidGoToXY(0, 0);
			HLCD_voidSendStringTypingEffect("Button Pressed", 50);
		}
	}
}

void vTaskLCD2(void *pvParameters) {
	(void) pvParameters;
	while (1) {
		HLCD_voidGoToXY(1, 0);
		HLCD_voidSendStringTypingEffect("Task 2", 50);
		vTaskDelay(1000);
		HLCD_voidGoToXY(1, 0);
		HLCD_voidSendString("                ");
	}
}

int main(void) {
	BaseType_t st;

	DIO_voidInit();
	HLCD_voidInit();
	EXTI_voidInit();
	EXTI_voidEnableGlobal();

	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
	DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

	xCounting_Semaphore = xSemaphoreCreateCounting(10U, 0U);
	if (xCounting_Semaphore == NULL) {
		while (1)
			;
	}



	HLCD_voidGoToXY(0, 0);
	HLCD_voidSendStringTypingEffect("FreeRTOS Demo", 50);
	DELAY_voidMs(1000);
	HLCD_voidClearScreen();

	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN6, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN7, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN0, DIO_OUTPUT);

	EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
	EXTI_u8SetCallback(EXTI_u8_INT0, Button_ISR);

	st = xTaskCreate(vTaskLed1, "LED1", TASK_STACK_INITIAL, NULL, 3U, NULL);
	if (st != pdPASS) {
		while (1)
			;
	}

	st = xTaskCreate(vTaskLed2, "LED2", TASK_STACK_INITIAL, NULL, 2U, NULL);
	if (st != pdPASS) {
		while (1)
			;
	}

	st = xTaskCreate(vTaskLed3, "LED3", TASK_STACK_INITIAL, NULL, 1U, NULL);
	if (st != pdPASS) {
		while (1)
			;
	}

	st = xTaskCreate(vTaskLCD1, "LCD1", TASK_STACK_INITIAL, NULL, 4U, NULL);
	if (st != pdPASS) {
		while (1)
			;
	}

	/*
	 * st = xTaskCreate(vTaskLCD2, "LCD2", TASK_STACK_INITIAL, NULL, 4U, NULL);
	 * if (st != pdPASS) { while(1); }
	 */

	vTaskStartScheduler();
	while (1)
		;
	return 0;
}
