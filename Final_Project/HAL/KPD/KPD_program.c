#include "../../CONFIG/KPD/KPD_config.h"
#include "../../HW/KPD/KPD_private.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/BIT_MATH.h"
#include "../../LIB/DELAY.h"
#include "../../MCAL/DIO/MDIO_interface.h"
#include "KPD_interface.h"

static u8 Local_u8KPDArr[KPD_NUM_ROWS][KPD_NUM_COLS] = KPD_KEYS;
static u8 Local_u8RowArr[KPD_NUM_ROWS] = {KPD_R0_PIN, KPD_R1_PIN, KPD_R2_PIN, KPD_R3_PIN};
static u8 Local_u8ColArr[KPD_NUM_COLS] = {KPD_C0_PIN, KPD_C1_PIN, KPD_C2_PIN, KPD_C3_PIN};

void KPD_voidInit(void) {
    u8 Local_u8RowIdx;
    u8 Local_u8ColIdx;

    for (Local_u8RowIdx = 0; Local_u8RowIdx < KPD_NUM_ROWS; Local_u8RowIdx++) {
        DIO_enumSetPinDirection(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_OUTPUT);
        DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_HIGH);
    }

    for (Local_u8ColIdx = 0; Local_u8ColIdx < KPD_NUM_COLS; Local_u8ColIdx++) {
        DIO_enumSetPinDirection(KPD_PORT, Local_u8ColArr[Local_u8ColIdx], DIO_INPUT);
        DIO_enumSetPinValue(KPD_PORT, Local_u8ColArr[Local_u8ColIdx], DIO_HIGH);
    }
}

u8 KPD_u8ScanKey(void) {
    u8 Local_u8RowIdx;
    u8 Local_u8ColIdx;
    u8 Local_u8PinState;

    for (Local_u8RowIdx = 0; Local_u8RowIdx < KPD_NUM_ROWS; Local_u8RowIdx++) {
        /* Activate current row (Active LOW) */
        DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_LOW);
        
        /* Tiny delay for signal to propagate through keypad matrix */
        DELAY_voidUs(4);

        /* Read columns */
        for (Local_u8ColIdx = 0; Local_u8ColIdx < KPD_NUM_COLS; Local_u8ColIdx++) {
            DIO_enumGetPinValue(KPD_PORT, Local_u8ColArr[Local_u8ColIdx], &Local_u8PinState);

            if (Local_u8PinState == DIO_LOW) {
                DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_HIGH);
                return Local_u8KPDArr[Local_u8RowIdx][Local_u8ColIdx];
            }
        }

        DIO_enumSetPinValue(KPD_PORT, Local_u8RowArr[Local_u8RowIdx], DIO_HIGH);
    }

    return KPD_NOT_PRESSED;
}

u8 KPD_u8GetPressedKey(void) {
    static u8 s_u8LastKey = KPD_NOT_PRESSED;
    u8 Local_u8CurrentKey = KPD_u8ScanKey();
    u8 Local_u8ValidKey = KPD_NOT_PRESSED;

    /* True non-blocking edge detection. 
       Debounce is naturally handled because this function is only 
       called every 20ms by the main super loop tick! */
    if (Local_u8CurrentKey != s_u8LastKey) {
        s_u8LastKey = Local_u8CurrentKey;
        Local_u8ValidKey = Local_u8CurrentKey;
    }

    return Local_u8ValidKey;
}
