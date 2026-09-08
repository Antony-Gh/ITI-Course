/*
 * main.c
 *
 * Assignment:
 * Using Timer 0 in the both OVF mode and CTC Mode to achieve the following
 * Requirements: Control a DC Motor to Rotate CW for 5 Secs, CCW for 3 Secs,
 * Stop for 5 Secs.
 *
 * NOTE: The Timer Driver (MTIMER) has been updated to support the 1ms tick
 * required by the Delay and Motor HAL in both OVF (Normal) and CTC modes.
 * You can switch between modes by changing MTIMER_TIMER0_MODE in
 * MTIMER_config.h to MTIMER_MODE_CTC or MTIMER_MODE_NORMAL.
 */

#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/STD_TYPES.h"

#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/SERVO/HSERVO_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

/* Hardware Configurations */
HDCMOTOR_t myMotor = {
    .Port = DIO_PORTD, .RelayCwPin = DIO_PIN0, .RelayCcwPin = DIO_PIN1};

/* Global Variables for LCD (Volatile for ISR safety) */
volatile u8 g_ServoAngle = 0;
volatile char *g_DcDirection = "CW  ";
volatile u8 g_DcSpeed = 100;
volatile u8 g_UpdateLcdFlag = 1;

/* ================================================================= */
/* TIMER 0 CALLBACK: Exclusively handles the DC Motor                */
/* Executes every 1ms                                                */
/* ================================================================= */
void Timer0_DC_Callback(void) {
  static u16 ms_dc_counter = 0;
  static u8 dcState = 0; /* 0: CW, 1: CCW, 2: STOP */

  ms_dc_counter++;

  if (dcState == 0 && ms_dc_counter >= 5000) {
    dcState = 1;
    ms_dc_counter = 0;
    g_DcDirection = "CCW ";
    g_DcSpeed = 100;
    HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CCW);
    g_UpdateLcdFlag = 1;
  } else if (dcState == 1 && ms_dc_counter >= 3000) {
    dcState = 2;
    ms_dc_counter = 0;
    g_DcDirection = "STOP";
    g_DcSpeed = 0;
    HDCMOTOR_voidStop(&myMotor);
    g_UpdateLcdFlag = 1;
  } else if (dcState == 2 && ms_dc_counter >= 5000) {
    dcState = 0;
    ms_dc_counter = 0;
    g_DcDirection = "CW  ";
    g_DcSpeed = 100;
    HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CW);
    g_UpdateLcdFlag = 1;
  }
}

/* ================================================================= */
/* TIMER 1 CALLBACK: Exclusively handles the Servo Motor             */
/* Executes on OCR1A Compare Match (Software PWM + 1s Counter)       */
/* ================================================================= */
void Timer1_Servo_Callback(void) {
  static u8 pwm_state = 0;
  static u8 pwm_cycle_counter = 0;
  static u16 current_pulse_us = 1000; /* Start at 0 degrees (1000us) */

  if (pwm_state == 0) {
    /* Start of PWM period: Set Pin HIGH */
    DIO_enumSetPinValue(HSERVO_PORT, HSERVO_PIN, DIO_HIGH);
    MTIMER_voidSetOcr1A(current_pulse_us);
    TCNT1 = 0; /* Force timer reset to guarantee exact timing */
    pwm_state = 1;
  } else {
    /* End of pulse: Set Pin LOW, wait for remainder of 20ms */
    DIO_enumSetPinValue(HSERVO_PORT, HSERVO_PIN, DIO_LOW);
    MTIMER_voidSetOcr1A(20000 - current_pulse_us);
    TCNT1 = 0; /* Force timer reset to guarantee exact timing */
    pwm_state = 0;

    /* 50 cycles * 20ms = 1000ms (1 Second) */
    pwm_cycle_counter++;
    if (pwm_cycle_counter >= 50) {
      pwm_cycle_counter = 0;

      /* Increment Angle */
      g_ServoAngle += 10;
      if (g_ServoAngle > 180) {
        g_ServoAngle = 0;
      }

      /* Update pulse width for the next cycle */
      current_pulse_us = 1000 + ((u32)g_ServoAngle * 1000) / 180;
      g_UpdateLcdFlag = 1;
    }
  }
}

/* ================================================================= */
/* Setup & Main Loop                                                 */
/* ================================================================= */
void setup(void) {
  DIO_voidInit();
  HLCD_voidInit();
  HDCMOTOR_voidInit(&myMotor);
  HSERVO_voidInit();

  /* Start initial DC Motor State */
  HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CW);

  /* Initialize Timers (Timer 0 = 1ms CTC, Timer 1 = 1us CTC) */
  MTIMER_voidInit();

  /* 1. Register Timer 0 Callback (DC Motor) */
#if (MTIMER_TIMER0_MODE == MTIMER_MODE_CTC)
  MTIMER_u8SetCompareCallback(MTIMER_TIMER0, Timer0_DC_Callback);
#else
  MTIMER_u8SetOverflowCallback(MTIMER_TIMER0, Timer0_DC_Callback);
#endif

  /* 2. Register Timer 1 Callback (Servo Motor) & Enable Interrupt */
  MTIMER_u8SetCompareCallback(MTIMER_TIMER1, Timer1_Servo_Callback);
  MTIMER_voidSetCompareIntState(MTIMER_TIMER1, 1);
}

void loop(void) {
  /* Non-blocking LCD Update */
  if (g_UpdateLcdFlag == 1) {
    g_UpdateLcdFlag = 0;

    /* Row 0: DC Motor Info */
    HLCD_voidGoToXY(0, 0);
    HLCD_voidSendString("DC: ");
    HLCD_voidSendString((char *)g_DcDirection);
    HLCD_voidSendString(" Spd:");
    HLCD_voidSendNumber(g_DcSpeed);
    HLCD_voidSendString("%  ");

    /* Row 1: Servo Motor Info */
    HLCD_voidGoToXY(1, 0);
    HLCD_voidSendString("Servo: ");
    HLCD_voidSendNumber(g_ServoAngle);
    HLCD_voidSendString(" deg   ");
  }
}

int main(void) {
  setup();

  while (1) {
    loop();
  }
  return 0;
}
