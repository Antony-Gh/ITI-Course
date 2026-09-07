/*
 * MTIMER_interface.h
 *
 *  Created on: Sep 2, 2026
 *      Author: Anthony Gaius
 */

#ifndef MTIMER_INTERFACE_H_
#define MTIMER_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

void initialize(void);
void enable_global_interrupts(void);





u32 get_millis(void);
u8 is_interval_elapsed(u32 *Copy_pu32LastMs, u16 Copy_u16IntervalMs);
void delay_ms(u16 Copy_u16DelayMs);



void set_compare_callback(void (*Copy_pvCallback)(void));
void set_overflow_callback(void (*Copy_pvCallback)(void));
void set_compare_int_state(u8 Copy_u8Enable);
void reset_timer_1(void);



#endif /* MTIMER_INTERFACE_H_ */
