#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

#include "../HAL/LED/HLED_interface.h"
#include "../HAL/KPD/KPD_interface.h"
#include "LED_animations.h"

#define ANIM_FRAME_DELAY_MS 120
#define ANIM_LOOP_COUNT     20

static void LED_voidDelayWithKeyScan(u8 Copy_u8CurrentKey, u8* Copy_pu8NewKey) {
    u8 Local_u8DelayIdx;
    u8 Local_u8ScannedKey;

    *Copy_pu8NewKey = Copy_u8CurrentKey;

    for (Local_u8DelayIdx = 0; Local_u8DelayIdx < ANIM_FRAME_DELAY_MS; Local_u8DelayIdx++) {
        Local_u8ScannedKey = KPD_u8ScanKey();
        if (Local_u8ScannedKey != KPD_NOT_PRESSED &&
            Local_u8ScannedKey != Copy_u8CurrentKey) {
            *Copy_pu8NewKey = Local_u8ScannedKey;
            return;
        }
        _delay_ms(1);
    }
}

static u8 LED_u8RunFrames(u8 Copy_u8CurrentKey, void (*Copy_pfnFrame)(u8*)) {
    u8 Local_u8LoopIdx;
    u8 Local_u8FrameState = 0;
    u8 Local_u8NewKey = Copy_u8CurrentKey;

    for (Local_u8LoopIdx = 0; Local_u8LoopIdx < ANIM_LOOP_COUNT; Local_u8LoopIdx++) {
        Copy_pfnFrame(&Local_u8FrameState);
        LED_voidDelayWithKeyScan(Copy_u8CurrentKey, &Local_u8NewKey);
        if (Local_u8NewKey != Copy_u8CurrentKey) {
            return Local_u8NewKey;
        }
    }

    return Copy_u8CurrentKey;
}

static void LED_voidAnimChaseRight(u8* Copy_pu8State) {
    HLED_voidWritePattern((u8)(1 << *Copy_pu8State));
    (*Copy_pu8State)++;
    if (*Copy_pu8State >= 8) {
        *Copy_pu8State = 0;
    }
}

static void LED_voidAnimChaseLeft(u8* Copy_pu8State) {
    HLED_voidWritePattern((u8)(1 << (7 - *Copy_pu8State)));
    (*Copy_pu8State)++;
    if (*Copy_pu8State >= 8) {
        *Copy_pu8State = 0;
    }
}

static void LED_voidAnimBounce(u8* Copy_pu8State) {
    static s8 Local_s8Direction = 1;
    u8 Local_u8LedIdx;

    if (*Copy_pu8State == 0) {
        Local_s8Direction = 1;
    } else if (*Copy_pu8State == 7) {
        Local_s8Direction = -1;
    }

    Local_u8LedIdx = *Copy_pu8State;
    HLED_voidWritePattern((u8)(1 << Local_u8LedIdx));
    *Copy_pu8State = (u8)(*Copy_pu8State + Local_s8Direction);
}

static void LED_voidAnimBlinkAll(u8* Copy_pu8State) {
    HLED_voidWritePattern((*Copy_pu8State == 0) ? 0xFF : 0x00);
    *Copy_pu8State = (*Copy_pu8State == 0) ? 1 : 0;
}

static void LED_voidAnimAlternate(u8* Copy_pu8State) {
    HLED_voidWritePattern((*Copy_pu8State == 0) ? 0x55 : 0xAA);
    *Copy_pu8State = (*Copy_pu8State == 0) ? 1 : 0;
}

static void LED_voidAnimBinaryCount(u8* Copy_pu8State) {
    HLED_voidWritePattern(*Copy_pu8State);
    (*Copy_pu8State)++;
}

static void LED_voidAnimExpandCenter(u8* Copy_pu8State) {
    static const u8 Local_u8Patterns[8] = {
        0x18, 0x3C, 0x7E, 0xFF, 0x7E, 0x3C, 0x18, 0x00
    };

    HLED_voidWritePattern(Local_u8Patterns[*Copy_pu8State]);
    (*Copy_pu8State)++;
    if (*Copy_pu8State >= 8) {
        *Copy_pu8State = 0;
    }
}

static void LED_voidAnimContractEdges(u8* Copy_pu8State) {
    static const u8 Local_u8Patterns[8] = {
        0x81, 0xC3, 0xE7, 0xFF, 0xE7, 0xC3, 0x81, 0x00
    };

    HLED_voidWritePattern(Local_u8Patterns[*Copy_pu8State]);
    (*Copy_pu8State)++;
    if (*Copy_pu8State >= 8) {
        *Copy_pu8State = 0;
    }
}

static void LED_voidAnimSplitChase(u8* Copy_pu8State) {
    u8 Local_u8Pattern;

    Local_u8Pattern = (u8)((1 << *Copy_pu8State) | (1 << (7 - *Copy_pu8State)));
    HLED_voidWritePattern(Local_u8Pattern);
    (*Copy_pu8State)++;
    if (*Copy_pu8State >= 4) {
        *Copy_pu8State = 0;
    }
}

void LED_voidRunAnimation(u8 Copy_u8Key) {
    u8 Local_u8CurrentKey = Copy_u8Key;

    while (1) {
        switch (Local_u8CurrentKey) {
            case '1':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimChaseRight);
                break;
            case '2':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimChaseLeft);
                break;
            case '3':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimBounce);
                break;
            case '4':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimBlinkAll);
                break;
            case '5':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimAlternate);
                break;
            case '6':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimBinaryCount);
                break;
            case '7':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimExpandCenter);
                break;
            case '8':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimContractEdges);
                break;
            case '9':
                Local_u8CurrentKey = LED_u8RunFrames(Local_u8CurrentKey, LED_voidAnimSplitChase);
                break;
            default:
                HLED_voidClearAll();
                return;
        }
    }
}
