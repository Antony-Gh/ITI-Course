#include <KPD/KPD_config.h>
#include <KPD/KPD_private.h>
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../MCAL/DIO/MDIO_interface.h"
#include "KPD_interface.h"
#include <util/delay.h>

void KPD_voidInit(void) {
    /* Set Rows as Output */
    DIO_enumSetPinDirection(KPD_PORT, KPD_R0_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_R1_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_R2_PIN, DIO_OUTPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_R3_PIN, DIO_OUTPUT);

    /* Set Columns as Input */
    DIO_enumSetPinDirection(KPD_PORT, KPD_C0_PIN, DIO_INPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_C1_PIN, DIO_INPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_C2_PIN, DIO_INPUT);
    DIO_enumSetPinDirection(KPD_PORT, KPD_C3_PIN, DIO_INPUT);

    /* Initialize Rows to HIGH (inactive) */
    DIO_enumSetPinValue(KPD_PORT, KPD_R0_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_R1_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_R2_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_R3_PIN, DIO_HIGH);

    /* Initialize Columns to HIGH (Enable Internal Pull-up Resistors) */
    DIO_enumSetPinValue(KPD_PORT, KPD_C0_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_C1_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_C2_PIN, DIO_HIGH);
    DIO_enumSetPinValue(KPD_PORT, KPD_C3_PIN, DIO_HIGH);
}

u8 KPD_u8GetPressedKey(void) {
    u8 Local_u8PressedKey = KPD_NOT_PRESSED;
    u8 Local_u8RowIdx, Local_u8ColIdx, Local_u8PinState;
    static u8 Local_u8KPDArr[4][4] = KPD_KEYS;
    static u8 Local_u8RowArr[4] = {KPD_R0_PIN, KPD_R1_PIN, KPD_R2_PIN, KPD_R3_PIN};
    static u8 Local_u8ColArr[4] = {KPD_C0_PIN, KPD_C1_PIN, KPD_C2_PIN, KPD_C3_PIN};

    for (Local_u8RowIdx = 0; Local_u8RowIdx < 4; Local_u8RowIdx++) {
        /* Activate current row (Active LOW) */
        DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_LOW);

        /* Read columns */
        for (Local_u8ColIdx = 0; Local_u8ColIdx < 4; Local_u8ColIdx++) {
            DIO_enumGetPinValue(KPD_PORT, Local_u8ColArr[Local_u8ColIdx], &Local_u8PinState);
            
            if (Local_u8PinState == DIO_LOW) {
                Local_u8PressedKey = Local_u8KPDArr[Local_u8RowIdx][Local_u8ColIdx];

                /* Wait until key is released (polling) to avoid multiple reads for single press */
                while (Local_u8PinState == DIO_LOW) {
                    DIO_enumGetPinValue(KPD_PORT, Local_u8ColArr[Local_u8ColIdx], &Local_u8PinState);
                }
                
                /* Debounce delay */
                _delay_ms(20);
                
                /* Deactivate current row before returning */
                DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_HIGH);
                return Local_u8PressedKey;
            }
        }

        /* Deactivate current row (HIGH) */
        DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_HIGH);
    }

    return Local_u8PressedKey;
}
