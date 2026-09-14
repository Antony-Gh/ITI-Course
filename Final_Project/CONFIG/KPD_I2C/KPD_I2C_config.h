/*
 * KPD_I2C_config.h
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  Keypad Configuration — I2C mode using TWO PCF8574 modules
 *  Supports an 8x8 keypad (64 keys)
 */

#ifndef KPD_I2C_CONFIG_H_
#define KPD_I2C_CONFIG_H_

#include "../../MCAL/I2C/MI2C_interface.h"

#define KPD_I2C_PCF_ROWS_ADDR       0x21U
#define KPD_I2C_PCF_COLS_ADDR       0x22U

#define KPD_I2C_NUM_ROWS            8U
#define KPD_I2C_NUM_COLS            8U

#define KPD_I2C_KEYS { \
    {'1', '2', '3', '4', '5', '6', '7', '8'}, \
    {'9', '0', 'W', 'S', 'E', 'C', 'U', 'D'}, \
    {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h'}, \
    {'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'}, \
    {'q', 'r', 's', 't', 'u', 'v', 'w', 'x'}, \
    {'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F'}, \
    {'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N'}, \
    {'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V'}  \
}

#endif /* KPD_I2C_CONFIG_H_ */
