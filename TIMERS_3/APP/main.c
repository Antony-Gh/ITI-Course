/*
 * main.c
 *
 *  Created on: Sep 02, 2026
 *      Author: Anthony Gaius
 *
 */

#include "../LIB/STD_TYPES.h"
#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../MCAL/DIO/MDIO_interface.h"

#include <avr/interrupt.h>
#include <util/delay.h>

void setup(void) {

	// DIO Initialization
	DIO_voidInit();

	// The pins we used PD5/OC1A & PD4/OC1B
	// PD4 & PD5 is connected to a motor with transistors and relays
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN4, DIO_OUTPUT);
	DIO_enumSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_OUTPUT);

	// F_CPU = 8MHz

	// For Fast PWM Mode 14:
	// Frequency of PWM = (F_CPU / N * (1 + ICR1))

	// ICR1 = ((F_CPU / (N * F_PWM)) - 1)

	// Total ICR1 = 8000

	// 37.5% duty cycle
	// OC1A = PD5 = 3000

	// 62.5% duty cycle
	// OC1B = PD4 = 5000

	// Timer/Counter1 Register TCNT1H and TCNT1L
	// Start Timer from 0
	TCNT1 = 0;

	// Input Capture Register 1
	// Timer counts
	ICR1 = 7999;

	// Output Compare Register 1 A
	// 3000 / 8000 = 37.5%
	OCR1A = 3000;

	// Output Compare Register 1 B
	// 5000 / 8000 = 62.5%
	OCR1B = 5000;


	// Clear the First Register (WGM13:0 bits)
	TCCR1A = 0;

	// Clear COM1A1:0
	// Set COM1A1:1
	// Non-inverting PWM on OC1A
    SET_BIT(TCCR1A, COM1A1);
    CLR_BIT(TCCR1A, COM1A0);

    // Clear COM1B1:0
	// Set COM1B1:1
	// Non-inverting PWM on OC1B
	SET_BIT(TCCR1A, COM1B1);
	CLR_BIT(TCCR1A, COM1B0);


    // Set WGM11 = 1
	// Clear WGM10 = 0
	// This is a part of Mode 14 (Fast PWM)
    SET_BIT(TCCR1A, WGM11);
    CLR_BIT(TCCR1A, WGM10);

    // So TCCR1A = 0b10100010;


    // Clear the Second Register (WGM13:0 bits)
    TCCR1B = 0;

    // WGM13:WGM12 = 11
    // WGM11:WGM10 = 10
    // All bits gives 1110 = 14

    // Set WGM13 = 1
    SET_BIT(TCCR1B, WGM13);

    // Set WGM13 = 1
    SET_BIT(TCCR1B, WGM12);

    // Set the timer to No prescaler CS10:1 = 001
    CLR_BIT(TCCR1B, CS12);
    CLR_BIT(TCCR1B, CS11);
    SET_BIT(TCCR1B, CS10);

    // So TCCR1B = 0b00011001;

	// Timer/Counter Interrupt Mask Register
	// Bit 5 – TICIE1: Timer/Counter1, Input Capture Interrupt Enable
	// Bit 4 – OCIE1A: Timer/Counter1, Output Compare A Match Interrupt Enable
	// Bit 3 – OCIE1B: Timer/Counter1, Output Compare B Match Interrupt Enable
	// Bit 2 – TOIE1: Timer/Counter1, Overflow Interrupt Enable
	TIMSK = 0b00000000;

	// Bit 5 – ICF1: Timer/Counter1, Input Capture Flag
	// Bit 4 – OCF1A: Timer/Counter1, Output Compare A Match Flag
	// Bit 3 – OCF1B: Timer/Counter1, Output Compare B Match Flag
	// Bit 2 – TOV1: Timer/Counter1, Overflow Flag

	// 0011 1100 = 0x3C
	// TIFR = 0x3C;
	TIFR = 0b00111100;

    _delay_ms(50);

}

void loop(void) {
	// Nothing
}

int main(void) {

	setup();

	while (1) {
		loop();
	}

	return 0;
}

ISR(TIMER1_COMPA_vect)
{

}
