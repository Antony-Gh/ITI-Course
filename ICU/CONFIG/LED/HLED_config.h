#ifndef HLED_CONFIG_H_
#define HLED_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"

/* Light-level LEDs on Port D (PD0=low, PD1=mid, PD2=high) */
#define HLED_PORT DIO_PORTD
#define HLED_NUM_LEDS 3

#endif /* HLED_CONFIG_H_ */
