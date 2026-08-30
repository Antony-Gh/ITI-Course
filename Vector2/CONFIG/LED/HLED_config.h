#ifndef HLED_CONFIG_H_
#define HLED_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/* Status LEDs on Port C (avoid conflict with LCD control pins on Port A) */
#define HLED_PORT DIO_PORTC
#define HLED_NUM_LEDS 2

#endif /* HLED_CONFIG_H_ */
