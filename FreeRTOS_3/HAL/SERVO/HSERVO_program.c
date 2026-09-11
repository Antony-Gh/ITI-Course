

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/TIMER/MTIMER_interface.h"

#include "../../CONFIG/SERVO/HSERVO_config.h"
#include "HSERVO_interface.h"

static volatile u16 s_u16CurrentPulseUs = HSERVO_MIN_PULSE_US;

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
