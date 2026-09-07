#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"

// Assuming your timer functions are exposed in a header file
#include "../MCAL/TIMER/MTIMER_interface.h"

volatile u16 my_timer_ms = 0;
volatile u8 my_motor_state = 0;

void overflow_function() {
  //   my_timer_ms++;

  //   if (my_motor_state == 0) {
  //     if (my_timer_ms >= 4000) {
  //       my_timer_ms = 0;
  //       my_motor_state = 1;
  //     }
  //   } else if (my_my_motor_state == 1) {
  //     if (my_timer_ms >= 3000) {
  //       my_timer_ms = 0;
  //       my_motor_state = 2;
  //     }
  //   } else if (my_my_motor_state == 2) {
  //     if (my_timer_ms >= 2000) {
  //       my_timer_ms = 0;
  //       my_motor_state = 0;
  //     }
  //   }
}

void compare_function() {
  my_timer_ms++;

  switch (my_motor_state) {

  case 0:

    if (my_timer_ms >= 800) {
      my_timer_ms = 0;
      my_motor_state = 1;
    }

    break;

  case 1:

    if (my_timer_ms >= 600) {
      my_timer_ms = 0;
      my_motor_state = 2;
    }

    break;

  case 2:

    if (my_timer_ms >= 400) {
      my_timer_ms = 0;
      my_motor_state = 0;
    }

    break;

  default:

    my_timer_ms = 0U;
    my_motor_state = 2;

    break;
  }
}

void setup(void) {

  DIO_voidInit();

  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN4, DIO_OUTPUT);
  DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN4, DIO_LOW);
  DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);

  initialize();

  set_overflow_callback(overflow_function);
  
  set_compare_callback(compare_function);

  
}

void loop(void) {
  switch (my_motor_state) {
  case 0:
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN4, DIO_HIGH);
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);
    break;
  case 1:
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN4, DIO_LOW);
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_HIGH);
    break;
  case 2:
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN4, DIO_LOW);
    DIO_enumSetPinValue(DIO_PORTD, DIO_PIN5, DIO_LOW);
    break;
  }
}

int main(void) {
  setup();

  while (1) {
    loop();
  }

  return 0;
}
