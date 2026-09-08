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

#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

/*
 * This enum is used to define the state of the motor.
 * It is used to switch between CW, CCW and STOP states.
 */
typedef enum { STATE_CW, STATE_CCW, STATE_STOP } MotorState_t;

/* Motor configuration matching HDCMOTOR_config.h */
HDCMOTOR_t myMotor = {
    .Port = DIO_PORTD, .RelayCwPin = DIO_PIN0, .RelayCcwPin = DIO_PIN1};

/* State Machine executed every 1 ms by the Timer Interrupt */
void MotorSequence_Callback(void) {
  static u16 ms_counter = 0;
  static MotorState_t currentState = STATE_CW;

  ms_counter++;

  switch (currentState) {
  case STATE_CW:
    if (ms_counter >= 5000) {
      currentState = STATE_CCW;
      ms_counter = 0;
      HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CCW);
    }
    break;

  case STATE_CCW:
    if (ms_counter >= 3000) {
      currentState = STATE_STOP;
      ms_counter = 0;
      HDCMOTOR_voidStop(&myMotor);
    }
    break;

  case STATE_STOP:
    if (ms_counter >= 5000) {
      currentState = STATE_CW;
      ms_counter = 0;
      HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CW);
    }
    break;
  }
}

void setup(void) {
  /* Initialize all required modules */
  DIO_voidInit();
  MTIMER_voidInit();
  HDCMOTOR_voidInit(&myMotor);

  /* Start the initial sequence */
  HDCMOTOR_voidRun(&myMotor, HDCMOTOR_CW);

  /* Register the callback depending on the configured Timer mode */
#if (MTIMER_TIMER0_MODE == MTIMER_MODE_CTC)
  /*
   * When using CTC mode, the Timer Compare Match interrupt (OCIE0) is enabled.
   * The MotorSequence_Callback will be executed every 1 ms.
   */
  MTIMER_u8SetCompareCallback(MTIMER_TIMER0, MotorSequence_Callback);
#elif (MTIMER_TIMER0_MODE == MTIMER_MODE_NORMAL)
  /*
   * When using Normal mode, the Timer Overflow interrupt (TOIE0) is enabled.
   * The MotorSequence_Callback will be executed every 1 ms.
   */
  MTIMER_u8SetOverflowCallback(MTIMER_TIMER0, MotorSequence_Callback);
#endif
}

void loop(void) {
  /* The main loop is completely non-blocking and free for other tasks */
}

int main(void) {

  setup();

  while (1) {
    loop();
  }

  return 0;
}
