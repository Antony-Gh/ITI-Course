/*
 * main.c  –  FreeRTOS Lab_1
 *
 *
 *  Created on : Sep 10, 2026
 *      Author : Anthony Gaius
 */

#include "../SERVICES/FreeRTOS/FreeRTOS.h"
#include "../SERVICES/FreeRTOS/task.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/DELAY.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../HAL/LCD/HLCD_interface.h"

#define TASK_STACK_INITIAL   150U
#define TASK_STACK_LARGER    400U

void vApplicationTickHook(void) {
	static uint16_t s_u16Count = 0;
	if (++s_u16Count >= 1000U) {
		s_u16Count = 0;
		PORTB ^= (1u << 0); /* PA0 toggles every 1000 ticks */
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
		HLCD_voidGoToXY(0, 1);
		HLCD_voidSendStringTypingEffect("Task 1", 50);
		vTaskDelay(1000);
		HLCD_voidGoToXY(0, 1);
		HLCD_voidSendString("                ");
	}
}

void vTaskLCD2(void *pvParameters) {
	(void) pvParameters;

	while (1) {
		HLCD_voidGoToXY(1, 1);
		HLCD_voidSendStringTypingEffect("Task 2", 50);
		vTaskDelay(1000);
		HLCD_voidGoToXY(1, 1);
		HLCD_voidSendString("                ");
	}
}

int main(void) {
	DIO_voidInit();
	HLCD_voidInit();
	HLCD_voidGoToXY(0, 1);
	HLCD_voidSendStringTypingEffect("FreeRTOS Demo", 50);
	DELAY_voidUs(1000);
	HLCD_voidClearScreen();

	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN6, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN7, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN0, DIO_OUTPUT);

	xTaskCreate(vTaskLed1, "LED1",
	TASK_STACK_INITIAL, NULL, 3U, NULL);

	xTaskCreate(vTaskLed2, "LED2",
	TASK_STACK_INITIAL, NULL, 2U, NULL);

	xTaskCreate(vTaskLed3, "LED3",
	TASK_STACK_INITIAL, NULL, 1U, NULL);

	xTaskCreate(vTaskLCD1, "LCD1",
	TASK_STACK_INITIAL, NULL, 4U, NULL);

	xTaskCreate(vTaskLCD2, "LCD2",
	TASK_STACK_INITIAL, NULL, 4U, NULL);

	vTaskStartScheduler();

	while (1)
		;
	return 0;
}
