/*
 * HSSD_config.h
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#ifndef HSSD_CONFIG_H_
#define HSSD_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/* Configure SSD Type: HSSD_COMMON_CATHODE_TYPE or HSSD_COMMON_ANODE_TYPE */
#define HSSD_TYPE  HSSD_COMMON_CATHODE_TYPE

/* Configure SSD Data Port (A-G and DP) */
#define HSSD_DATA_PORT  DIO_PORTA

/* Configure SSD Enable Pin (Common Pin for multiplexing) */
#define HSSD_EN_PORT    DIO_PORTB
#define HSSD_EN_PIN     DIO_PIN0

#endif /* HSSD_CONFIG_H_ */
