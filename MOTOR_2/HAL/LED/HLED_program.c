#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../CONFIG/LED/HLED_config.h"
#include "../../HW/LED/HLED_private.h"
#include "HLED_interface.h"

void HLED_voidInit(void) {
    DIO_enumSetPortDirection(HLED_PORT, DIO_PORT_OUTPUT);
    HLED_voidClearAll();
}

void HLED_voidWritePattern(u8 Copy_u8Pattern) {
    DIO_enumSetPortValue(HLED_PORT, Copy_u8Pattern);
}

void HLED_voidSetLed(u8 Copy_u8LedIdx, u8 Copy_u8State) {
    if (Copy_u8LedIdx < HLED_NUM_LEDS) {
        DIO_enumSetPinValue(HLED_PORT, Copy_u8LedIdx, Copy_u8State);
    }
}

void HLED_voidClearAll(void) {
    DIO_enumSetPortValue(HLED_PORT, 0x00);
}
