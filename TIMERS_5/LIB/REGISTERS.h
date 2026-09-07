/*
 * REGISTERS.h
 *
 *  Created on: Sep 1, 2026
 *      Author: anthonygaius
 *      Target: ATmega32A
 */

#ifndef REGISTERS_H_
#define REGISTERS_H_

#include "STD_TYPES.h" // Ensure u8 and u16 are defined

/* =========================================================
 *                   CPU REGISTERS
 * ========================================================= */

/*
 * Status Register
 * I   T   H   S   V   N   Z   C
 * 7   6   5   4   3   2   1   0
 */
#define SREG        (*(volatile u8*)0x5F)
#define SREG_I      7
#define SREG_T      6
#define SREG_H      5
#define SREG_S      4
#define SREG_V      3
#define SREG_N      2
#define SREG_Z      1
#define SREG_C      0

/* Stack Pointer */
#define SPH         (*(volatile u8*)0x5E)
#define SPL         (*(volatile u8*)0x5D)
#define SP          (*(volatile u16*)0x5D)

/* =========================================================
 *                   GPIO - PORT A
 * ========================================================= */
#define PORTA       (*(volatile u8*)0x3B)
#define DDRA        (*(volatile u8*)0x3A)
#define PINA        (*(volatile u8*)0x39)

#define PA7         7
#define PA6         6
#define PA5         5
#define PA4         4
#define PA3         3
#define PA2         2
#define PA1         1
#define PA0         0

/* =========================================================
 *                   GPIO - PORT B
 * ========================================================= */
#define PORTB       (*(volatile u8*)0x38)
#define DDRB        (*(volatile u8*)0x37)
#define PINB        (*(volatile u8*)0x36)

#define PB7         7
#define PB6         6
#define PB5         5
#define PB4         4
#define PB3         3
#define PB2         2
#define PB1         1
#define PB0         0

/* =========================================================
 *                   GPIO - PORT C
 * ========================================================= */
#define PORTC       (*(volatile u8*)0x35)
#define DDRC        (*(volatile u8*)0x34)
#define PINC        (*(volatile u8*)0x33)

#define PC7         7
#define PC6         6
#define PC5         5
#define PC4         4
#define PC3         3
#define PC2         2
#define PC1         1
#define PC0         0

/* =========================================================
 *                   GPIO - PORT D
 * ========================================================= */
#define PORTD       (*(volatile u8*)0x32)
#define DDRD        (*(volatile u8*)0x31)
#define PIND        (*(volatile u8*)0x30)

#define PD7         7
#define PD6         6
#define PD5         5
#define PD4         4
#define PD3         3
#define PD2         2
#define PD1         1
#define PD0         0

/* =========================================================
 *                TIMER / COUNTER SHARED INTERRUPTS
 * ========================================================= */
/* Timer/Counter Interrupt Mask */
#define TIMSK       (*(volatile u8*)0x59)
#define OCIE2       7
#define TOIE2       6
#define TICIE1      5
#define OCIE1A      4
#define OCIE1B      3
#define TOIE1       2
#define OCIE0       1
#define TOIE0       0

/* Timer/Counter Interrupt Flag */
#define TIFR        (*(volatile u8*)0x58)
#define OCF2        7
#define TOV2        6
#define ICF1        5
#define OCF1A       4
#define OCF1B       3
#define TOV1        2
#define OCF0        1
#define TOV0        0

/* =========================================================
 *                  TIMER / COUNTER 0
 * ========================================================= */
#define TCCR0       (*(volatile u8*)0x53)
#define FOC0        7
#define WGM00       6
#define COM01       5
#define COM00       4
#define WGM01       3
#define CS02        2
#define CS01        1
#define CS00        0

#define TCNT0       (*(volatile u8*)0x52)
#define OCR0        (*(volatile u8*)0x5C)

/* =========================================================
 *                  TIMER / COUNTER 1
 * ========================================================= */
#define TCCR1A      (*(volatile u8*)0x4F)
#define COM1A1      7
#define COM1A0      6
#define COM1B1      5
#define COM1B0      4
#define FOC1A       3
#define FOC1B       2
#define WGM11       1
#define WGM10       0

#define TCCR1B      (*(volatile u8*)0x4E)
#define ICNC1       7
#define ICES1       6
#define WGM13       4
#define WGM12       3
#define CS12        2
#define CS11        1
#define CS10        0

#define TCNT1L      (*(volatile u8*)0x4C)
#define TCNT1H      (*(volatile u8*)0x4D)
#define TCNT1       (*(volatile u16*)0x4C)

#define OCR1AL      (*(volatile u8*)0x4A)
#define OCR1AH      (*(volatile u8*)0x4B)
#define OCR1A       (*(volatile u16*)0x4A)

#define OCR1BL      (*(volatile u8*)0x48)
#define OCR1BH      (*(volatile u8*)0x49)
#define OCR1B       (*(volatile u16*)0x48)

#define ICR1L       (*(volatile u8*)0x46)
#define ICR1H       (*(volatile u8*)0x47)
#define ICR1        (*(volatile u16*)0x46)

/* =========================================================
 *                  TIMER / COUNTER 2
 * ========================================================= */
#define TCCR2       (*(volatile u8*)0x45)
#define FOC2        7
#define WGM20       6
#define COM21       5
#define COM20       4
#define WGM21       3
#define CS22        2
#define CS21        1
#define CS20        0

#define TCNT2       (*(volatile u8*)0x44)
#define OCR2        (*(volatile u8*)0x43)
#define ASSR        (*(volatile u8*)0x42)

/* =========================================================
 *                        ADC
 * ========================================================= */
#define ADMUX       (*(volatile u8*)0x27)
#define REFS1       7
#define REFS0       6
#define ADLAR       5
#define MUX4        4
#define MUX3        3
#define MUX2        2
#define MUX1        1
#define MUX0        0

#define ADCSRA      (*(volatile u8*)0x26)
#define ADEN        7
#define ADSC        6
#define ADATE       5
#define ADIF        4
#define ADIE        3
#define ADPS2       2
#define ADPS1       1
#define ADPS0       0

#define ADCL        (*(volatile u8*)0x24)
#define ADCH        (*(volatile u8*)0x25)
#define ADC         (*(volatile u16*)0x24)

#define SFIOR       (*(volatile u8*)0x50)
#define ADTS2       7
#define ADTS1       6
#define ADTS0       5

/* =========================================================
 *                   ANALOG COMPARATOR
 * ========================================================= */
#define ACSR        (*(volatile u8*)0x28)
#define ACD         7
#define ACBG        6
#define ACO         5
#define ACI         4
#define ACIE        3
#define ACIC        2
#define ACIS1       1
#define ACIS0       0

/* =========================================================
 *                        USART
 * ========================================================= */
#define UBRRL       (*(volatile u8*)0x29)
#define UBRRH       (*(volatile u8*)0x40) // Shared address with UCSRC
#define UBRR        (*(volatile u16*)0x29)

#define UCSRA       (*(volatile u8*)0x2B)
#define RXC         7
#define TXC         6
#define UDRE        5
#define FE          4
#define DOR         3
#define PE          2
#define U2X         1
#define MPCM        0

#define UCSRB       (*(volatile u8*)0x2A)
#define RXCIE       7
#define TXCIE       6
#define UDRIE       5
#define RXEN        4
#define TXEN        3
#define UCSZ2       2
#define RXB8        1
#define TXB8        0

#define UCSRC       (*(volatile u8*)0x40) // Shared address with UBRRH
#define URSEL       7
#define UMSEL       6
#define UPM1        5
#define UPM0        4
#define USBS        3
#define UCSZ1       2
#define UCSZ0       1
#define UCPOL       0

#define UDR         (*(volatile u8*)0x2C)

/* =========================================================
 *                        SPI
 * ========================================================= */
#define SPCR        (*(volatile u8*)0x2D)
#define SPIE        7
#define SPE         6
#define DORD        5
#define MSTR        4
#define CPOL        3
#define CPHA        2
#define SPR1        1
#define SPR0        0

#define SPSR        (*(volatile u8*)0x2E)
#define SPIF        7
#define WCOL        6
#define SPI2X       0

#define SPDR        (*(volatile u8*)0x2F)

/* =========================================================
 *                        TWI / I2C
 * ========================================================= */
#define TWBR        (*(volatile u8*)0x20)

#define TWSR        (*(volatile u8*)0x21)
#define TWS7        7
#define TWS6        6
#define TWS5        5
#define TWS4        4
#define TWS3        3
#define TWPS1       1
#define TWPS0       0

#define TWAR        (*(volatile u8*)0x22)
#define TWA6        7
#define TWA5        6
#define TWA4        5
#define TWA3        4
#define TWA2        3
#define TWA1        2
#define TWA0        1
#define TWGCE       0

#define TWDR        (*(volatile u8*)0x23)

#define TWCR        (*(volatile u8*)0x56)
#define TWINT       7
#define TWEA        6
#define TWSTA       5
#define TWSTO       4
#define TWWC        3
#define TWEN        2
#define TWIE        0

/* =========================================================
 *                   EXTERNAL INTERRUPTS
 * ========================================================= */
#define MCUCR       (*(volatile u8*)0x55)
#define SE          7
#define SM2         6
#define SM1         5
#define SM0         4
#define ISC11       3
#define ISC10       2
#define ISC01       1
#define ISC00       0

#define MCUCSR      (*(volatile u8*)0x54)
#define JTD         7
#define ISC2        6
#define JTRF        4
#define WDRF        3
#define BORF        2
#define EXTRF       1
#define PORF        0

#define GICR        (*(volatile u8*)0x5B)
#define INT1        7
#define INT0        6
#define INT2        5
#define IVSEL       1
#define IVCE        0

#define GIFR        (*(volatile u8*)0x5A)
#define INTF1       7
#define INTF0       6
#define INTF2       5

/* =========================================================
 *                  WATCHDOG TIMER
 * ========================================================= */
#define WDTCR       (*(volatile u8*)0x41)
#define WDTOE       4
#define WDE         3
#define WDP2        2
#define WDP1        1
#define WDP0        0

/* =========================================================
 *                      EEPROM
 * ========================================================= */
#define EEARL       (*(volatile u8*)0x3E)
#define EEARH       (*(volatile u8*)0x3F)
#define EEAR        (*(volatile u16*)0x3E)

#define EEDR        (*(volatile u8*)0x3D)

#define EECR        (*(volatile u8*)0x3C)
#define EERIE       3
#define EEMWE       2
#define EEWE        1
#define EERE        0

#endif /* REGISTERS_H_ */
