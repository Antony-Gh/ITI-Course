#ifndef HSERVO_CONFIG_H_
#define HSERVO_CONFIG_H_

/* Servo Motor Hardware Connection */
#define HSERVO_PORT DIO_PORTD
#define HSERVO_PIN DIO_PIN5

/* Standard Servo Pulse Constraints (in microseconds) */
#define HSERVO_MIN_PULSE_US 1000U /* Pulse for 0 degrees */
#define HSERVO_MAX_PULSE_US 2000U /* Pulse for 180 degrees */
#define HSERVO_PERIOD_US 20000U   /* 50Hz PWM Period */

#define HSERVO_PORT DIO_PORTD
#define HSERVO_PIN  DIO_PIN5

#endif /* HSERVO_CONFIG_H_ */
