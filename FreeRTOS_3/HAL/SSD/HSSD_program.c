/*
 * HSSD_program.c
 *
 *  Created on: Aug 25, 2026
 *      Author: Anthony Gaius
 */

#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"

#include "../../MCAL/DIO/MDIO_interface.h"

#include "../../HW/SSD/HSSD_private.h"
#include "../../CONFIG/SSD/HSSD_config.h"
#include "HSSD_interface.h"

static const u8 HSSD_u8NumbersCC[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};
static const u8 HSSD_u8NumbersCA[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

void HSSD_voidInit(void) {
    /* Set Data Port as Output */
    DIO_enumSetPortDirection(HSSD_DATA_PORT, DIO_PORT_OUTPUT);
    
    /* Set Enable Pin as Output */
    DIO_enumSetPinDirection(HSSD_EN_PORT, HSSD_EN_PIN, DIO_OUTPUT);
}

void HSSD_voidDisplayNumber(u8 Copy_u8Number) {
    if (Copy_u8Number < 10) {
#if HSSD_TYPE == HSSD_COMMON_CATHODE_TYPE
        DIO_enumSetPortValue(HSSD_DATA_PORT, HSSD_u8NumbersCC[Copy_u8Number]);
#elif HSSD_TYPE == HSSD_COMMON_ANODE_TYPE
        DIO_enumSetPortValue(HSSD_DATA_PORT, HSSD_u8NumbersCA[Copy_u8Number]);
#endif
    }
}

void HSSD_voidEnable(void) {
#if HSSD_TYPE == HSSD_COMMON_CATHODE_TYPE
    DIO_enumSetPinValue(HSSD_EN_PORT, HSSD_EN_PIN, DIO_LOW);
#elif HSSD_TYPE == HSSD_COMMON_ANODE_TYPE
    DIO_enumSetPinValue(HSSD_EN_PORT, HSSD_EN_PIN, DIO_HIGH);
#endif
}

void HSSD_voidDisable(void) {
#if HSSD_TYPE == HSSD_COMMON_CATHODE_TYPE
    DIO_enumSetPinValue(HSSD_EN_PORT, HSSD_EN_PIN, DIO_HIGH);
#elif HSSD_TYPE == HSSD_COMMON_ANODE_TYPE
    DIO_enumSetPinValue(HSSD_EN_PORT, HSSD_EN_PIN, DIO_LOW);
#endif
}
