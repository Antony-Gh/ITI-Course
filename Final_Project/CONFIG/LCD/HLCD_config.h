/*
 * HLCD_config.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 *
 *  LCD Configuration — 4-bit mode on PORTC
 *
 *  PORTA is reserved for the 8-bit DAC output.
 *  LCD operates in 4-bit mode using PORTC upper nibble for data
 *  and PORTC lower pins for control.
 *
 *  IMPORTANT: JTAG must be disabled (JTAGEN fuse = unprogrammed)
 *  to free PC2–PC5 for GPIO use.
 */

#ifndef HLCD_CONFIG_H_
#define HLCD_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/* ====================================================================
 *  LCD Mode Selection
 *  Options: HLCD_MODE_4BIT / HLCD_MODE_8BIT
 * ==================================================================== */
#define HLCD_MODE_4BIT    4U
#define HLCD_MODE_8BIT    8U
#define HLCD_MODE         HLCD_MODE_4BIT

/* ====================================================================
 *  LCD Control Pins — PORTC lower bits
 * ==================================================================== */
#define HLCD_CTRL_PORT    DIO_PORTC
#define HLCD_RS_PIN       DIO_PIN0
#define HLCD_RW_PIN       DIO_PIN1
#define HLCD_EN_PIN       DIO_PIN2

/* ====================================================================
 *  LCD Data Pins — PORTC upper nibble (4-bit mode)
 *  D4 = PC4, D5 = PC5, D6 = PC6, D7 = PC7
 * ==================================================================== */
#define HLCD_DATA_PORT    DIO_PORTC
#define HLCD_D4_PIN       DIO_PIN4
#define HLCD_D5_PIN       DIO_PIN5
#define HLCD_D6_PIN       DIO_PIN6
#define HLCD_D7_PIN       DIO_PIN7

#endif /* HLCD_CONFIG_H_ */
