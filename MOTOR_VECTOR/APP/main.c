/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Assignment: Toggle DC motor ON/OFF using a push button on an EXTI pin.
 * ATmega32 INT0 = PD2 (falling edge, internal pull-up, button to GND).
 */

#include "../LIB/STD_TYPES.h"

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../HAL/LED/HLED_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

static u8 s_u8PowerState = 0U;
static u8 s_u8PreviousPowerState = 0xFFU;
static volatile u8 s_u8TogglePending = 0U;

static void APP_voidToggleIsr(void) {
  s_u8TogglePending = 1U;
}

static void APP_voidUpdateIndicators(void) {
#if APP_USE_LED
  HLED_voidSetLed(APP_LED_POWER_IDX, s_u8PowerState);
  HLED_voidSetLed(APP_LED_OFF_IDX, (s_u8PowerState == 0U) ? DIO_HIGH : DIO_LOW);
#endif
}

static void APP_voidApplyMotorCommand(HDCMOTOR_t *Copy_pstMotor) {
  if (s_u8PowerState == s_u8PreviousPowerState) {
    return;
  }

  if (s_u8PowerState == 0U) {
    (void)HDCMOTOR_enumRequestStop(Copy_pstMotor);
  } else {
    (void)HDCMOTOR_enumRequestRun(Copy_pstMotor, HDCMOTOR_CW);
  }

  s_u8PreviousPowerState = s_u8PowerState;
}

static void APP_voidProcessToggleRequest(void) {
  u8 local_u8ButtonState;

  if (s_u8TogglePending == 0U) {
    return;
  }

  /* Debounce: latch toggle only after the button is released (HIGH). */
  if (DIO_enumGetPinValue(APP_TOGGLE_BUTTON_PORT, APP_TOGGLE_BUTTON_PIN,
                         &local_u8ButtonState) == DIO_OK &&
      local_u8ButtonState == DIO_HIGH) {
    s_u8TogglePending = 0U;
    s_u8PowerState ^= 1U;
    APP_voidUpdateIndicators();
  }
}

int main(void) {
  HDCMOTOR_t local_stMotor;

  MTIMER_voidInit();
  DIO_voidInit();
  EXTI_voidInit();

#if APP_USE_LED
  HLED_voidInit();
#endif

  (void)DIO_enumSetPinDirection(APP_TOGGLE_BUTTON_PORT, APP_TOGGLE_BUTTON_PIN,
                                 DIO_INPUT);
  (void)DIO_enumSetPinValue(APP_TOGGLE_BUTTON_PORT, APP_TOGGLE_BUTTON_PIN,
                            DIO_HIGH);

  local_stMotor.Port = HDCMOTOR_RELAY_PORT;
  local_stMotor.RelayCwPin = HDCMOTOR_RELAY_CW_PIN;
  local_stMotor.RelayCcwPin = HDCMOTOR_RELAY_CCW_PIN;
  
  (void)HDCMOTOR_enumInit(&local_stMotor);

  (void)EXTI_u8SetCallback(APP_TOGGLE_EXTI_ID, APP_voidToggleIsr);
  EXTI_voidEnableGlobal();

  APP_voidUpdateIndicators();

  while (1) {
    APP_voidProcessToggleRequest();
    APP_voidApplyMotorCommand(&local_stMotor);
    HDCMOTOR_voidUpdate(&local_stMotor);
  }

  return 0;
}
