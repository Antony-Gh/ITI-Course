/*
 * MI2C_private.h
 *
 *  Created on: Sep 9, 2026
 *      Author: anthonygaius
 */

#ifndef HW_I2C_MI2C_PRIVATE_H_
#define HW_I2C_MI2C_PRIVATE_H_

#define MI2C_STATUS_MASK             0xF8

#define MI2C_START_TRANSMITTED      0x08
#define MI2C_REP_START_TRANSMITTED  0x10

#define MI2C_SLA_W_ACK              0x18
#define MI2C_SLA_W_NACK             0x20

#define MI2C_DATA_W_ACK             0x28
#define MI2C_DATA_W_NACK            0x30

#define MI2C_SLA_R_ACK              0x40
#define MI2C_SLA_R_NACK             0x48

#define MI2C_DATA_R_ACK             0x50
#define MI2C_DATA_R_NACK            0x58

#define MI2C_BUS_ERROR              0x00
#define MI2C_ARB_LOST               0x38

#endif /* HW_I2C_MI2C_PRIVATE_H_ */
