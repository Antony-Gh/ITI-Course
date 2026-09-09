/*
 * main.c
 *
 * Assignment: Using 2 ATMEGA 32 Microcontrollers to control buzzer’s tone.
 * The first one reads a compare match value (OC0) Selected via keypad and sends
 * it to the second one via UART. The second one uses this value to generate a
 * PWM Signal produces this tone on the buzzer.
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/STD_TYPES.h"

#include "../HAL/KPD/KPD_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"
#include "../MCAL/USART/MUSART_interface.h"

#define MCU_1_TX 1
#define MCU_2_RX 2

/* --- SELECT THE ACTIVE MCU HERE --- */
#define ACTIVE_MCU MCU_2_RX
/* ---------------------------------- */

#if ACTIVE_MCU == MCU_1_TX

/*
 * MCU 1 (Transmitter)
 * Reads OCR0 value from keypad and sends it via UART.
 */
int main(void) {
  DIO_voidInit();
  HLCD_voidInit();
  KPD_voidInit();
  USART_voidInit();

  HLCD_voidSendString("Enter OC0 val:");
  HLCD_voidGoToXY(1, 0);
  HLCD_voidSendString("Val: 0   ");
  HLCD_voidGoToXY(1, 5);

  u16 current_val = 0;

  while (1) {
    u8 key = KPD_u8GetPressedKey();
    if (key != KPD_NOT_PRESSED) {
      if (key >= '0' && key <= '9') {
        current_val = (current_val * 10) + (key - '0');
        if (current_val > 255) {
          current_val = 255; /* Cap at max OCR0 value */
        }
        HLCD_voidGoToXY(1, 0);
        HLCD_voidSendString("Val: ");
        HLCD_voidSendNumber(current_val);
        HLCD_voidSendString("    ");
      } else if (key == '=') { /* '=' acts as Enter/Send */
        USART_enumTransmit((u8)current_val);
        HLCD_voidGoToXY(1, 0);
        HLCD_voidSendString("Sent: ");
        HLCD_voidSendNumber(current_val);
        HLCD_voidSendString("    ");
        current_val = 0;       /* Reset for next input */
      } else if (key == 'C') { /* 'C' acts as Clear */
        current_val = 0;
        HLCD_voidGoToXY(1, 0);
        HLCD_voidSendString("Cleared     ");
        MTIMER_voidDelayMs(500);
        HLCD_voidGoToXY(1, 0);
        HLCD_voidSendString("Val: 0      ");
      }
    }
  }
  return 0;
}

#elif ACTIVE_MCU == MCU_2_RX

/*
 * MCU 2 (Receiver)
 * Receives OCR0 value via UART and uses it to generate PWM (Tone) on buzzer.
 *
 * IMPORTANT: Ensure the following configurations are set in your drivers:
 * 1. MTIMER_config.h: #define MTIMER_TIMER0_MODE MTIMER_MODE_FAST_PWM
 */
int main(void) {
  DIO_voidInit();
  USART_voidInit();

  /* Initialize Timers (Timer0 will start in Fast PWM mode if configured so) */
  MTIMER_voidInit();

  /* Set PB3 (OC0) as output for PWM signal to the buzzer */
  DIO_enumSetPinDirection(DIO_PORTB, DIO_PIN3, DIO_OUTPUT);

  /* Initial Duty Cycle 0% */
  MTIMER_voidSetOcr0(0);

  u8 received_val = 0;
  MTIMER_voidSetOcr0(received_val);

  while (1) {
    if (USART_enumReceive(&received_val) == USART_OK) {
      /* Update OCR0 to change the PWM duty cycle (Tone) */
      MTIMER_voidSetOcr0(received_val);
    }
  }
  return 0;
}

#endif
