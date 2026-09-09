/*
 * MSPI_config.h
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      Compile-time configuration for the ATmega32 SPI driver.
 *      Select the desired options and rebuild.
 */

#ifndef MSPI_CONFIG_H_
#define MSPI_CONFIG_H_

/*
 * SPI Mode (Role)
 *
 * This value is overridden automatically by APP/main.c based on
 * ACTIVE_MCU, so you do NOT need to change it manually between builds.
 * It only acts as a fallback default when used standalone.
 *
 * Options:
 *   - 0U : SPI_MODE_SLAVE
 *   - 1U : SPI_MODE_MASTER
 */
#ifndef SPI_MODE
#define SPI_MODE  1U   /* default: master (overridden by main.c) */
#endif

/*
 * Data Order
 * Options:
 *   - 0U : SPI_DATA_ORDER_MSB_FIRST  (MSB sent first – standard)
 *   - 1U : SPI_DATA_ORDER_LSB_FIRST
 */
#define SPI_DATA_ORDER  0U

/*
 * Clock Polarity (CPOL)
 * Options:
 *   - 0U : SPI_CPOL_IDLE_LOW   (SCK idle = LOW  → SPI Mode 0 or 1)
 *   - 1U : SPI_CPOL_IDLE_HIGH  (SCK idle = HIGH → SPI Mode 2 or 3)
 */
#define SPI_CPOL  0U

/*
 * Clock Phase (CPHA)
 * Options:
 *   - 0U : SPI_CPHA_LEAD_SAMPLE    (Sample on leading edge  → SPI Mode 0 or 2)
 *   - 1U : SPI_CPHA_TRAIL_SAMPLE   (Sample on trailing edge → SPI Mode 1 or 3)
 */
#define SPI_CPHA  0U

/*
 * SPI Clock Prescaler
 * Actual SCK = F_CPU / prescaler
 * Options (SPCR[SPR1:SPR0] combined with SPSR[SPI2X]):
 *   - 0U : SPI_PRESCALER_4    (2X=0, SPR=00) → 8 MHz / 4   = 2.000 MHz
 *   - 1U : SPI_PRESCALER_16   (2X=0, SPR=01) → 8 MHz / 16  = 500.0 kHz
 *   - 2U : SPI_PRESCALER_64   (2X=0, SPR=10) → 8 MHz / 64  = 125.0 kHz
 *   - 3U : SPI_PRESCALER_128  (2X=0, SPR=11) → 8 MHz / 128 = 62.50 kHz
 *   - 4U : SPI_PRESCALER_2    (2X=1, SPR=00) → 8 MHz / 2   = 4.000 MHz
 *   - 5U : SPI_PRESCALER_8    (2X=1, SPR=01) → 8 MHz / 8   = 1.000 MHz
 *   - 6U : SPI_PRESCALER_32   (2X=1, SPR=10) → 8 MHz / 32  = 250.0 kHz
 *   - 7U : SPI_PRESCALER_64X  (2X=1, SPR=11) → 8 MHz / 64  = 125.0 kHz (same as 2)
 */
#define SPI_PRESCALER  1U   /* 500 kHz – safe for Proteus simulation */

/*
 * SPI Interrupt Enable
 * Options:
 *   - 0U : SPI_INTERRUPT_DISABLE  (polling / blocking mode)
 *   - 1U : SPI_INTERRUPT_ENABLE   (ISR-driven mode)
 */
#define SPI_INTERRUPT_ENABLE  0U

/*
 * Timeout guard for blocking Transmit / Receive (loop iterations).
 * Prevents the MCU from hanging forever if the SPI bus stalls.
 * A value of 50000 covers ~25 ms @ 8 MHz with a tight loop.
 */
#define SPI_TIMEOUT_COUNT  50000UL

#endif /* MSPI_CONFIG_H_ */
