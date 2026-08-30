/*
 * main.c
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 *
 * Application: 2-Relay H-Bridge DC Motor Direction Control
 */

#include "../LIB/STD_TYPES.h"

#include "../CONFIG/APP/APP_config.h"
#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../HAL/LED/HLED_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/PB/HPB_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

static u8 s_u8PowerState = 0U;
static u8 s_u8CurrentDirection = HDCMOTOR_CW;
static u8 s_u8PreviousPowerState = 0xFFU;
static u8 s_u8PreviousDirection = 0xFFU;

static void APP_voidUpdateIndicators(void) {
#if APP_USE_LED
  HLED_voidSetLed(APP_LED_POWER_IDX, s_u8PowerState);
  HLED_voidSetLed(APP_LED_DIRECTION_IDX,
                  (s_u8PowerState != 0U &&
                   s_u8CurrentDirection == HDCMOTOR_CCW)
                      ? DIO_HIGH
                      : DIO_LOW);
#endif

#if APP_USE_LCD
  HLCD_voidGoToXY(0, 0);
  if (s_u8PowerState != 0U) {
    HLCD_voidSendString("Motor: ON   ");
  } else {
    HLCD_voidSendString("Motor: OFF  ");
  }

  HLCD_voidGoToXY(1, 0);
  if (s_u8PowerState == 0U) {
    HLCD_voidSendString("Dir:  ---    ");
  } else if (s_u8CurrentDirection == HDCMOTOR_CW) {
    HLCD_voidSendString("Dir:  CW     ");
  } else {
    HLCD_voidSendString("Dir:  CCW    ");
  }
#endif
}

static void APP_voidApplyMotorCommand(HDCMOTOR_t *Copy_pstMotor) {
  if (s_u8PowerState == s_u8PreviousPowerState &&
      (s_u8PowerState == 0U ||
       s_u8CurrentDirection == s_u8PreviousDirection)) {
    return;
  }

  if (s_u8PowerState == 0U) {
    (void)HDCMOTOR_enumRequestStop(Copy_pstMotor);
  } else {
    (void)HDCMOTOR_enumRequestRun(Copy_pstMotor, s_u8CurrentDirection);
  }

  s_u8PreviousPowerState = s_u8PowerState;
  s_u8PreviousDirection = s_u8CurrentDirection;
}

int main(void) {
  HPB_t local_stOffButton;
  HPB_t local_stModeButton;
  HDCMOTOR_t local_stMotor;
  u8 local_u8Edge;
  u8 local_u8DisplayDirty = 1U;

  MTIMER_voidInit();
  DIO_voidInit();

#if APP_USE_LED
  HLED_voidInit();
#endif

#if APP_USE_LCD
  HLCD_voidInit();
  HLCD_voidClearScreen();
#endif

  (void)HPB_enumInit(&local_stOffButton, APP_OFF_BUTTON_PORT,
                     APP_OFF_BUTTON_PIN, HPB_PULL_UP);
  (void)HPB_enumInit(&local_stModeButton, APP_MODE_BUTTON_PORT,
                     APP_MODE_BUTTON_PIN, HPB_PULL_UP);

  local_stMotor.Port = HDCMOTOR_RELAY_PORT;
  local_stMotor.RelayCwPin = HDCMOTOR_RELAY_CW_PIN;
  local_stMotor.RelayCcwPin = HDCMOTOR_RELAY_CCW_PIN;
  (void)HDCMOTOR_enumInit(&local_stMotor);

  APP_voidUpdateIndicators();

  while (1) {
    HPB_voidUpdate(&local_stOffButton);
    HPB_voidUpdate(&local_stModeButton);

    if (HPB_enumGetEdge(&local_stOffButton, &local_u8Edge) == OK &&
        local_u8Edge == HPB_EDGE_PRESSED) {
      if (s_u8PowerState != 0U) {
        s_u8PowerState = 0U;
        s_u8CurrentDirection = HDCMOTOR_CW;
      } else {
        s_u8PowerState = 1U;
      }
      local_u8DisplayDirty = 1U;
    }

    if (s_u8PowerState != 0U &&
        HPB_enumGetEdge(&local_stModeButton, &local_u8Edge) == OK &&
        local_u8Edge == HPB_EDGE_PRESSED) {
      if (s_u8CurrentDirection == HDCMOTOR_CW) {
        s_u8CurrentDirection = HDCMOTOR_CCW;
      } else {
        s_u8CurrentDirection = HDCMOTOR_CW;
      }
      local_u8DisplayDirty = 1U;
    }

    APP_voidApplyMotorCommand(&local_stMotor);
    HDCMOTOR_voidUpdate(&local_stMotor);

    if (local_u8DisplayDirty != 0U) {
      APP_voidUpdateIndicators();
      local_u8DisplayDirty = 0U;
    }
  }

  return 0;
}
