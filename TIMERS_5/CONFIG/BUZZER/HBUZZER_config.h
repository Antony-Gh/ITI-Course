/*
 * HBUZZER_config.h
 *
 *  Created on: Sep 1, 2026
 *      Author: Anthony Gaius
 */

#ifndef HBUZZER_CONFIG_H_
#define HBUZZER_CONFIG_H_

#include "../../MCAL/DIO/MDIO_interface.h"
#include "../../MCAL/TIMER/MTIMER_interface.h"

/* Buzzer on PB1 (Timer1 OC1A) — standard Proteus wiring */
#define HBUZZER_PORT DIO_PORTB
#define HBUZZER_PIN DIO_PIN1
#define HBUZZER_TIMER_ID MTIMER_u8_TIMER1

/* Tone frequencies (Hz) for pot sections 0 (low), 1 (mid), 2 (high) */
#define HBUZZER_TONE_SECTION0_HZ 400U
#define HBUZZER_TONE_SECTION1_HZ 800U
#define HBUZZER_TONE_SECTION2_HZ 1200U

#define HBUZZER_NUM_TONES 3U

#endif /* HBUZZER_CONFIG_H_ */
