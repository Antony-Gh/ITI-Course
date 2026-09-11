/*
 * main.c  –  FreeRTOS Lab_3
 *
 * Assignment:
 *   Toggle 3 LEDs at different intervals (1 s, 3 s, 5 s) using FreeRTOS.
 *   A push button on EXTI0 (PD2) pauses / resumes the entire system.
 *
 * Hardware:
 *   LED1  -> PD5   (toggles every 1 s)
 *   LED2  -> PD6   (toggles every 3 s)
 *   LED3  -> PD7   (toggles every 5 s)
 *   Button-> PD2   (INT0, active-low with internal pull-up)
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
#define TASK_STACK_SIZE 150U

/* Toggle periods in milliseconds (1 tick = 1 ms) */
#define LED1_PERIOD_MS 1000U /* 1 second  */
#define LED2_PERIOD_MS 3000U /* 3 seconds */
#define LED3_PERIOD_MS 5000U /* 5 seconds */

/* Task priorities */
#define PRIORITY_LED 1U    /* All LED tasks share the same priority */
#define PRIORITY_BUTTON 2U /* Button task pre-empts LED tasks        */

/* ───────────────────── Global task handles ─────────────────────────── */
static TaskHandle_t xLed1Handle = NULL;
static TaskHandle_t xLed2Handle = NULL;
static TaskHandle_t xLed3Handle = NULL;

/*
 * Binary semaphore used for ISR -> vTaskButtonHandler communication.
 * The ISR gives the semaphore; the handler task takes it and
 * toggles the suspended / running state of all LED tasks.
 */
static SemaphoreHandle_t xButtonSem = NULL;

/* ─────────────────── EXTI0 ISR  (button on PD2) ───────────────────── */
void Button_ISR(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  /* Unblock vTaskButtonHandler from interrupt context */
  xSemaphoreGiveFromISR(xButtonSem, &xHigherPriorityTaskWoken);

  /* Trigger an immediate context-switch if a higher-priority task
   * was woken (the button handler has PRIORITY_BUTTON > PRIORITY_LED) */
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/* ─────────────────── Button-handler task ───────────────────────────── */
/*
 * Blocks on the binary semaphore indefinitely.
 * Each successful Take toggles between "system paused" and "system running"
 * by calling vTaskSuspend / vTaskResume on every LED task.
 */
void vTaskButtonHandler(void *pvParameters) {
  (void)pvParameters;

  static u8 u8Paused = 0U; /* 0 = running, 1 = paused */

  while (1) {
    /* Wait for a button press */
    if (xSemaphoreTake(xButtonSem, portMAX_DELAY) == pdTRUE) {
      if (u8Paused == 0U) {
        /* ── Pause all LED tasks ── */
        vTaskSuspend(xLed1Handle);
        vTaskSuspend(xLed2Handle);
        vTaskSuspend(xLed3Handle);
        u8Paused = 1U;
      } else {
        /* ── Resume all LED tasks ── */
        vTaskResume(xLed1Handle);
        vTaskResume(xLed2Handle);
        vTaskResume(xLed3Handle);
        u8Paused = 0U;
      }
    }
  }
}

/* ─────────────────── LED Task 1 – toggles every 1 s ───────────────── */
void vTaskLed1(void *pvParameters) {
  (void)pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN5);
    /* vTaskDelayUntil guarantees a drift-free periodic execution */
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(LED1_PERIOD_MS));
  }
}

/* ─────────────────── LED Task 2 – toggles every 3 s ───────────────── */
void vTaskLed2(void *pvParameters) {
  (void)pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN6);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(LED2_PERIOD_MS));
  }
}

/* ─────────────────── LED Task 3 – toggles every 5 s ───────────────── */
void vTaskLed3(void *pvParameters) {
  (void)pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1) {
    DIO_enumTogglePinValue(DIO_PORTD, DIO_PIN7);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(LED3_PERIOD_MS));
  }
}

/* ──────────────────────────── main ────────────────────────────────── */
int main(void) {
  BaseType_t xStatus;

  /* ── GPIO init ── */
  DIO_voidInit();

  /* LED output pins – start LOW (OFF) */
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN6, DIO_OUTPUT);
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN7, DIO_OUTPUT);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN6, DIO_LOW);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN7, DIO_LOW);

  /* Button: input with internal pull-up (active-low) */
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN2, DIO_HIGH);

  /* ── EXTI init ── */
  EXTI_voidInit();
  EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
  EXTI_u8SetCallback(EXTI_u8_INT0, Button_ISR);
  EXTI_voidEnableGlobal();

  /* ── Create binary semaphore for ISR -> button task signalling ── */
  xButtonSem = xSemaphoreCreateBinary();
  if (xButtonSem == NULL) {
    while (1)
      ;
  }

  /* ── Create LED tasks (store handles for suspend/resume) ── */
  xStatus = xTaskCreate(vTaskLed1, "LED1", TASK_STACK_SIZE, NULL, PRIORITY_LED,
                        &xLed1Handle);
  if (xStatus != pdPASS) {
    while (1)
      ;
  }

  xStatus = xTaskCreate(vTaskLed2, "LED2", TASK_STACK_SIZE, NULL, PRIORITY_LED,
                        &xLed2Handle);
  if (xStatus != pdPASS) {
    while (1)
      ;
  }

  xStatus = xTaskCreate(vTaskLed3, "LED3", TASK_STACK_SIZE, NULL, PRIORITY_LED,
                        &xLed3Handle);
  if (xStatus != pdPASS) {
    while (1)
      ;
  }

  /* ── Create button-handler task (higher priority so it pre-empts LEDs) ── */
  xStatus = xTaskCreate(vTaskButtonHandler, "BTN", TASK_STACK_SIZE, NULL,
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
