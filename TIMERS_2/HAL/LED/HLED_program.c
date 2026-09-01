#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../CONFIG/LED/HLED_config.h"
#include "../../HW/LED/HLED_private.h"
#include "HLED_interface.h"

void HLED_voidInit(void) {
    u8 local_u8LedIdx;

    for (local_u8LedIdx = 0U; local_u8LedIdx < HLED_NUM_LEDS; local_u8LedIdx++) {
        DIO_enumSetPinDirection(HLED_PORT, local_u8LedIdx, DIO_OUTPUT);
    }
    HLED_voidClearAll();
}

void HLED_voidWritePattern(u8 Copy_u8Pattern) {
    u8 local_u8LedIdx;

    for (local_u8LedIdx = 0U; local_u8LedIdx < HLED_NUM_LEDS; local_u8LedIdx++) {
        HLED_voidSetLed(local_u8LedIdx,
                        (Copy_u8Pattern & (1U << local_u8LedIdx)) ? DIO_HIGH
                                                                  : DIO_LOW);
    }
}

void HLED_voidSetLed(u8 Copy_u8LedIdx, u8 Copy_u8State) {
    if (Copy_u8LedIdx < HLED_NUM_LEDS) {
        DIO_enumSetPinValue(HLED_PORT, Copy_u8LedIdx, Copy_u8State);
    }
}

void HLED_voidClearAll(void) {
    u8 local_u8LedIdx;

    for (local_u8LedIdx = 0U; local_u8LedIdx < HLED_NUM_LEDS; local_u8LedIdx++) {
        HLED_voidSetLed(local_u8LedIdx, DIO_LOW);
    }
}

void HLED_voidSetSection(u8 Copy_u8Section) {
    u8 local_u8Pattern;

    if (Copy_u8Section >= (HLED_NUM_LEDS - 1U)) {
        local_u8Pattern = HLED_PORT_MASK;
    } else {
        local_u8Pattern = (u8)((1U << (Copy_u8Section + 1U)) - 1U);
    }

    PORTD = (PORTD & (u8)(~HLED_PORT_MASK)) | local_u8Pattern;
}
