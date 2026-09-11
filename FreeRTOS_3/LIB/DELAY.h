/*
 * DELAY.h
 *
 *  Timer-free blocking delays for AVR.
 *
 *  Uses AVR-GCC's built-in __builtin_avr_delay_cycles() — no external
 *  header, no timer, no interrupt. Pure CPU busy-wait.
 *
 *  Requirements:
 *      - F_CPU must be defined to the actual CPU frequency (Hz).
 *      - The function argument must be a compile-time constant to
 *        __builtin_avr_delay_cycles, so we split the runtime value
 *        into a loop of small constant delays.
 *
 *  WARNING: These functions BLOCK the CPU. Never call them from
 *           inside a FreeRTOS task — use vTaskDelay() there.
 *           Use them only in drivers / init code that runs before
 *           the scheduler starts or when blocking is acceptable.
 */

#ifndef DELAY_H_
#define DELAY_H_

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/STD_TYPES.h"

/* -----------------------------------------------------------------
 * Number of CPU cycles per microsecond / millisecond.
 * These are integer constants because F_CPU is a constant.
 * ----------------------------------------------------------------- */
#define DELAY_CYCLES_PER_US    ( F_CPU / 1000000UL )   /* 8 at 8 MHz   */
#define DELAY_CYCLES_PER_MS    ( F_CPU / 1000UL    )   /* 8000 at 8 MHz */

/* -----------------------------------------------------------------
 * Microsecond delay
 *   Each iteration burns DELAY_CYCLES_PER_US cycles (constant).
 *   The while-loop overhead adds ~3–5 cycles per iteration, which
 *   slightly LENGTHENS the delay — safe for setup/hold timings.
 * ----------------------------------------------------------------- */
static inline void DELAY_voidUs(u16 Copy_u16Us)
{
    while (Copy_u16Us--)
    {
        __builtin_avr_delay_cycles( DELAY_CYCLES_PER_US );
    }
}

/* -----------------------------------------------------------------
 * Millisecond delay
 * ----------------------------------------------------------------- */
static inline void DELAY_voidMs(u16 Copy_u16Ms)
{
    while (Copy_u16Ms--)
    {
        __builtin_avr_delay_cycles( DELAY_CYCLES_PER_MS );
    }
}

#endif /* DELAY_H_ */
