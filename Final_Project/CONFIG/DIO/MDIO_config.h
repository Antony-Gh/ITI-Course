/*
 * DIO_config.h
 *
 *  Created on: Aug 24, 2026
 *      Author: Anthony Gaius
 *
 *  DIO Configuration for Function Generator project.
 *
 *  PORTA: All output → DAC0808 (D0..D7)
 *  PORTB: Default input (SPI pins available if needed)
 *  PORTC: Mixed — PC0..PC2 output (LCD ctrl), PC4..PC7 output (LCD data 4-bit)
 *         PC3 free
 *  PORTD: PD2..PD4 input with pull-up (buttons),
 *         PD5 output (OC1A HW square wave)
 */

#ifndef DIO_CONFIG_H_
#define DIO_CONFIG_H_

/*
 * Pin Direction Configuration
 * Options:
 *   - DIO_INPUT
 *   - DIO_OUTPUT
 */

/* PORTA — 8-bit DAC output */
#define DIO_PORTA_PIN0_DIR DIO_OUTPUT
#define DIO_PORTA_PIN1_DIR DIO_OUTPUT
#define DIO_PORTA_PIN2_DIR DIO_OUTPUT
#define DIO_PORTA_PIN3_DIR DIO_OUTPUT
#define DIO_PORTA_PIN4_DIR DIO_OUTPUT
#define DIO_PORTA_PIN5_DIR DIO_OUTPUT
#define DIO_PORTA_PIN6_DIR DIO_OUTPUT
#define DIO_PORTA_PIN7_DIR DIO_OUTPUT

/* PORTB — Default input (available for SPI / future expansion) */
#define DIO_PORTB_PIN0_DIR DIO_INPUT
#define DIO_PORTB_PIN1_DIR DIO_INPUT
#define DIO_PORTB_PIN2_DIR DIO_INPUT
#define DIO_PORTB_PIN3_DIR DIO_INPUT
#define DIO_PORTB_PIN4_DIR DIO_INPUT
#define DIO_PORTB_PIN5_DIR DIO_INPUT
#define DIO_PORTB_PIN6_DIR DIO_INPUT
#define DIO_PORTB_PIN7_DIR DIO_INPUT

/* PORTC — LCD 4-bit mode (ctrl: PC0-PC2 output, data: PC4-PC7 output) */
#define DIO_PORTC_PIN0_DIR DIO_OUTPUT
#define DIO_PORTC_PIN1_DIR DIO_OUTPUT
#define DIO_PORTC_PIN2_DIR DIO_OUTPUT
#define DIO_PORTC_PIN3_DIR DIO_INPUT
#define DIO_PORTC_PIN4_DIR DIO_OUTPUT
#define DIO_PORTC_PIN5_DIR DIO_OUTPUT
#define DIO_PORTC_PIN6_DIR DIO_OUTPUT
#define DIO_PORTC_PIN7_DIR DIO_OUTPUT

/* PORTD — Buttons (PD2-PD4 input), OC1A (PD5 output) */
#define DIO_PORTD_PIN0_DIR DIO_INPUT
#define DIO_PORTD_PIN1_DIR DIO_INPUT
#define DIO_PORTD_PIN2_DIR DIO_INPUT
#define DIO_PORTD_PIN3_DIR DIO_INPUT
#define DIO_PORTD_PIN4_DIR DIO_INPUT
#define DIO_PORTD_PIN5_DIR DIO_OUTPUT
#define DIO_PORTD_PIN6_DIR DIO_INPUT
#define DIO_PORTD_PIN7_DIR DIO_INPUT

/*
 * Pin Value Configuration
 * Options:
 *   - DIO_LOW  (or Float if direction is INPUT)
 *   - DIO_HIGH (or Pull-up if direction is INPUT)
 */

/* PORTA — DAC starts at 0 */
#define DIO_PORTA_PIN0_VAL DIO_LOW
#define DIO_PORTA_PIN1_VAL DIO_LOW
#define DIO_PORTA_PIN2_VAL DIO_LOW
#define DIO_PORTA_PIN3_VAL DIO_LOW
#define DIO_PORTA_PIN4_VAL DIO_LOW
#define DIO_PORTA_PIN5_VAL DIO_LOW
#define DIO_PORTA_PIN6_VAL DIO_LOW
#define DIO_PORTA_PIN7_VAL DIO_LOW

/* PORTB — Float */
#define DIO_PORTB_PIN0_VAL DIO_LOW
#define DIO_PORTB_PIN1_VAL DIO_LOW
#define DIO_PORTB_PIN2_VAL DIO_LOW
#define DIO_PORTB_PIN3_VAL DIO_LOW
#define DIO_PORTB_PIN4_VAL DIO_LOW
#define DIO_PORTB_PIN5_VAL DIO_LOW
#define DIO_PORTB_PIN6_VAL DIO_LOW
#define DIO_PORTB_PIN7_VAL DIO_LOW

/* PORTC — LCD pins start low */
#define DIO_PORTC_PIN0_VAL DIO_LOW
#define DIO_PORTC_PIN1_VAL DIO_LOW
#define DIO_PORTC_PIN2_VAL DIO_LOW
#define DIO_PORTC_PIN3_VAL DIO_LOW
#define DIO_PORTC_PIN4_VAL DIO_LOW
#define DIO_PORTC_PIN5_VAL DIO_LOW
#define DIO_PORTC_PIN6_VAL DIO_LOW
#define DIO_PORTC_PIN7_VAL DIO_LOW

/* PORTD — PD2..PD4 pull-up for buttons, rest low */
#define DIO_PORTD_PIN0_VAL DIO_LOW
#define DIO_PORTD_PIN1_VAL DIO_LOW
#define DIO_PORTD_PIN2_VAL DIO_HIGH
#define DIO_PORTD_PIN3_VAL DIO_HIGH
#define DIO_PORTD_PIN4_VAL DIO_HIGH
#define DIO_PORTD_PIN5_VAL DIO_LOW
#define DIO_PORTD_PIN6_VAL DIO_LOW
#define DIO_PORTD_PIN7_VAL DIO_LOW

#endif /* DIO_CONFIG_H_ */
