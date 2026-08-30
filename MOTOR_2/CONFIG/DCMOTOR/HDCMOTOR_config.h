/*
 * HDCMOTOR_config.h
 *
 *  Created on: Aug 27, 2026
 *      Author: Anthony Gaius
 */

#ifndef HDCMOTOR_CONFIG_H_
#define HDCMOTOR_CONFIG_H_

/*
 * 2-Relay H-Bridge with Transistor Isolation
 * -------------------------------------------
 * Each relay coil is driven through an NPN transistor so the MCU is
 * electrically isolated from the motor supply.
 *
 *   AVR Pin ----[1 kOhm]---- Base (NPN)
 *                              |
 *                           Emitter -> GND
 *                           Collector -> Relay Coil -> Motor Supply (+)
 *                           Flyback Diode across coil (cathode to +)
 *
 * Relay K1 (CW)  : reverses motor terminal wiring for clockwise rotation
 * Relay K2 (CCW) : reverses motor terminal wiring for counter-clockwise
 *
 * Truth table (only one relay may be ON at a time):
 *   K1   K2   Motor
 *   OFF  OFF  Stop
 *   ON   OFF  CW
 *   OFF  ON   CCW
 *   ON   ON   Forbidden (short-circuit risk)
 */

#define HDCMOTOR_RELAY_PORT DIO_PORTD
#define HDCMOTOR_RELAY_CW_PIN DIO_PIN0  /* PD0 -> Q1 -> Relay K1 */
#define HDCMOTOR_RELAY_CCW_PIN DIO_PIN1 /* PD1 -> Q2 -> Relay K2 */

#define HDCMOTOR_RELAY_ACTIVE_LEVEL DIO_HIGH
#define HDCMOTOR_RELAY_INACTIVE_LEVEL DIO_LOW

/* Delay after de-energizing a relay before energizing the other one */
#define HDCMOTOR_RELAY_SWITCH_DELAY_MS 50U

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#endif /* HDCMOTOR_CONFIG_H_ */
