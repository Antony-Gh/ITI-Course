/*
 * main.c
 *
 *  Created on: Sep 02, 2026
 *      Author: Anthony Gaius
 *
 * Software ICU:
 *   Timer0 CTC 1 ms  -> 50 Hz / 45% PWM on PB3
 *   Timer1 Normal    -> free-running 1 us ticks
 *   EXTI0            -> INT0 / PD2 (PWM input)
 * Jump PB3 to PD2, then frequency and duty cycle are shown on the LCD.
 */

#include <util/delay.h>

#include "../CONFIG/APP/APP_config.h"
#include "../CONFIG/EXTI/MEXTI_config.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/EXTI/MEXTI_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

#define APP_ICU_EDGE_FIRST_RISING 0U
#define APP_ICU_EDGE_FALLING 1U
#define APP_ICU_EDGE_SECOND_RISING 2U

static volatile u8 s_u8IcuEdge = APP_ICU_EDGE_FIRST_RISING;
static volatile u8 s_u8MeasurementReady = 0U;
static volatile u8 s_u8PwmMs = 0U;
static volatile u32 s_u32TonTicks = 0U;
static volatile u32 s_u32PeriodTicks = 0U;
static volatile u32 s_u32T1Ticks = 0U;
static volatile u32 s_u32T2Ticks = 0U;

static void APP_voidPwmTick(void) {
	s_u8PwmMs++;
	if (s_u8PwmMs >= APP_PWM_PERIOD_MS) {
		s_u8PwmMs = 0U;
	}
	if (s_u8PwmMs < APP_PWM_ON_MS) {
		DIO_enumSetPinValue(APP_PWM_PORT, APP_PWM_PIN, DIO_HIGH);
	} else {
		DIO_enumSetPinValue(APP_PWM_PORT, APP_PWM_PIN, DIO_LOW);
	}
}

static void APP_voidIcuIsr(void) {
	u32 local_u32NowTicks = MTIMER_u32GetTimer1Ticks();

	if (s_u8IcuEdge == APP_ICU_EDGE_FIRST_RISING) {
		s_u32T1Ticks = local_u32NowTicks;
		EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
		s_u8IcuEdge = APP_ICU_EDGE_FALLING;
	} else if (s_u8IcuEdge == APP_ICU_EDGE_FALLING) {
		s_u32T2Ticks = local_u32NowTicks;
		EXTI_u8SetSense(EXTI_u8_INT0, EXTI_RISING_EDGE);
		s_u8IcuEdge = APP_ICU_EDGE_SECOND_RISING;
	} else {
		s_u32PeriodTicks = local_u32NowTicks - s_u32T1Ticks;
		s_u32TonTicks = s_u32T2Ticks - s_u32T1Ticks;
		s_u8MeasurementReady = 1U;

		s_u32T1Ticks = local_u32NowTicks;
		EXTI_u8SetSense(EXTI_u8_INT0, EXTI_FALLING_EDGE);
		s_u8IcuEdge = APP_ICU_EDGE_FALLING;
	}
}

static void APP_voidDisplayMeasurement(u32 Copy_u32FrequencyHz, u32 Copy_u32DutyPercent) {
	HLCD_voidGoToXY(0U, 0U);
	HLCD_voidSendString("Freq: ");
	HLCD_voidSendNumber(Copy_u32FrequencyHz);
	HLCD_voidSendString(" Hz   ");

	HLCD_voidGoToXY(1U, 0U);
	HLCD_voidSendString("Duty: ");
	HLCD_voidSendNumber(Copy_u32DutyPercent);
	HLCD_voidSendString(" %    ");
}

void setup(void) {
	DIO_voidInit();
	DIO_enumSetPinDirection(APP_PWM_PORT, APP_PWM_PIN, DIO_OUTPUT);
	DIO_enumSetPinValue(APP_PWM_PORT, APP_PWM_PIN, DIO_LOW);
	DIO_enumSetPinDirection(APP_ICU_PORT, APP_ICU_PIN, DIO_INPUT);
	HLCD_voidInit();
	EXTI_u8SetCallback(EXTI_u8_INT0, APP_voidIcuIsr);
	EXTI_voidInit();
	MTIMER_u8SetCompareCallback(MTIMER_u8_TIMER0, APP_voidPwmTick);
	MTIMER_voidInit();
}

void loop(void) {
	u32 local_u32PeriodTicks;
	u32 local_u32TonTicks;
	u32 local_u32FrequencyHz;
	u32 local_u32DutyPercent;
	if (s_u8MeasurementReady == 0U) {
		return;
	}
	local_u32PeriodTicks = s_u32PeriodTicks;
	local_u32TonTicks = s_u32TonTicks;
	if (local_u32PeriodTicks == 0U) {
		return;
	}
	local_u32FrequencyHz =
		F_CPU / ((u32)MTIMER_TIMER1_PRESCALER_VALUE * local_u32PeriodTicks);
	local_u32DutyPercent = (local_u32TonTicks * 100UL) / local_u32PeriodTicks;
	APP_voidDisplayMeasurement(local_u32FrequencyHz, local_u32DutyPercent);
	_delay_ms(250U);
}

int main(void) {
	setup();

	while (1) {
		loop();
	}

	return 0;
}
