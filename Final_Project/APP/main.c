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

#include "../HAL/LCD/HLCD_interface.h"
#include "../LIB/BIT_MATH.h"
#include "../LIB/DELAY.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"

#define STACK_LED 150U
#define STACK_BUTTON 250U

#define LED1_PERIOD_MS 1000U
#define LED2_PERIOD_MS 3000U
#define LED3_PERIOD_MS 5000U

#define PRIORITY_LED 1U
#define PRIORITY_BUTTON 2U

SemaphoreHandle_t xButtonSem = NULL;

static TaskHandle_t xLed1Handle = NULL;
static TaskHandle_t xLed2Handle = NULL;
static TaskHandle_t xLed3Handle = NULL;

void Button_ISR(void) {

  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  // Give the counting semaphore
  xSemaphoreGiveFromISR(xButtonSem, &xHigherPriorityTaskWoken);

  (void)xHigherPriorityTaskWoken;
}

void vTaskLCD(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    if (xSemaphoreTake(xButtonSem, portMAX_DELAY) == pdTRUE) {
      HLCD_voidClearScreen();
      HLCD_voidSendStringTypingEffect("Button Pressed", 20);
      vTaskDelay(1000);
      HLCD_voidClearScreen();
    }
  }
}

void vTaskLed1(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN5);
    vTaskDelay(LED1_PERIOD_MS);
  }
}

void vTaskLed2(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN6);
    vTaskDelay(LED2_PERIOD_MS);
  }
}

void vTaskLed3(void *pvParameters) {
  (void)pvParameters;

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
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

  xButtonSem = xSemaphoreCreateCounting(10, 0);
  if (xButtonSem == NULL) {
    while (1)
      ;
  }

  HLCD_voidInit();

  HLCD_voidSendStringTypingEffect("Initializing...", 20);
  DELAY_voidMs(1000);
  HLCD_voidClearScreen();

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

  xStatus =
      xTaskCreate(vTaskLCD, "LCD", STACK_BUTTON, NULL, PRIORITY_BUTTON, NULL);
  if (xStatus != pdPASS) {
    while (1)
      ;
  }

  vTaskStartScheduler();
  while (1)
    ;
  return 0;
}
