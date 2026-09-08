

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/TIMER/MTIMER_interface.h"

#include "../../CONFIG/SERVO/HSERVO_config.h"
#include "HSERVO_interface.h"

static volatile u16 s_u16CurrentPulseUs = HSERVO_MIN_PULSE_US;

/* Software PWM State Machine - Triggered by Timer 1 */
static void HSERVO_voidPwmCallback(void) {
  static u8 pwm_state = 0;

  if (pwm_state == 0) {
    /* Start of 20ms period: Set Pin HIGH */
    DIO_enumSetPinValue(HSERVO_PORT, HSERVO_PIN, DIO_HIGH);
    MTIMER_voidSetOcr1A(s_u16CurrentPulseUs);
    pwm_state = 1;
  } else {
    /* End of pulse: Set Pin LOW, wait for remainder of 20ms */
    DIO_enumSetPinValue(HSERVO_PORT, HSERVO_PIN, DIO_LOW);
    MTIMER_voidSetOcr1A(HSERVO_PERIOD_US - s_u16CurrentPulseUs);
    pwm_state = 0;
  }
}

void HSERVO_voidInit(void) {
  /* Configure Servo Pin as Output */
  DIO_enumSetPinDirection(HSERVO_PORT, HSERVO_PIN, DIO_OUTPUT);
}

void HSERVO_voidSetAngle(u8 Copy_u8Angle) {
  if (Copy_u8Angle > 180) {
    Copy_u8Angle = 180;
  }

  /* Map Angle (0-180) to Pulse Width (1000us - 2000us) */
  s_u16CurrentPulseUs =
      HSERVO_MIN_PULSE_US +
      ((u32)Copy_u8Angle * (HSERVO_MAX_PULSE_US - HSERVO_MIN_PULSE_US)) / 180;
}
