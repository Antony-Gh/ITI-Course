/*
 * MSPI_private.h
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      Internal (private) computed constants for the SPI driver.
 *      Never include this file outside of MSPI_program.c.
 */

#ifndef MSPI_PRIVATE_H_
#define MSPI_PRIVATE_H_

#include "../../CONFIG/SPI/MSPI_config.h"

/* -----------------------------------------------------------------------
 * Validate configuration values at compile-time
 * --------------------------------------------------------------------- */
#if (SPI_MODE != 0U) && (SPI_MODE != 1U)
#error "MSPI_config.h: SPI_MODE must be 0 (SLAVE) or 1 (MASTER)"
#endif

#if (SPI_PRESCALER > 7U)
#error "MSPI_config.h: SPI_PRESCALER must be in range 0..7"
#endif

/* -----------------------------------------------------------------------
 * Resolve SPCR[SPR1:SPR0] and SPSR[SPI2X] from SPI_PRESCALER
 *
 *   SPI_PRESCALER | SPI2X | SPR1 | SPR0 | Divisor
 *   ------------- | ----- | ---- | ---- | -------
 *        0        |   0   |   0  |   0  |   /4
 *        1        |   0   |   0  |   1  |  /16
 *        2        |   0   |   1  |   0  |  /64
 *        3        |   0   |   1  |   1  | /128
 *        4        |   1   |   0  |   0  |   /2
 *        5        |   1   |   0  |   1  |   /8
 *        6        |   1   |   1  |   0  |  /32
 *        7        |   1   |   1  |   1  |  /64
 * --------------------------------------------------------------------- */
#if   (SPI_PRESCALER == 0U)
  #define SPI_PRIVATE_SPR_BITS  0x00U
  #define SPI_PRIVATE_SPI2X     0U
#elif (SPI_PRESCALER == 1U)
  #define SPI_PRIVATE_SPR_BITS  0x01U
  #define SPI_PRIVATE_SPI2X     0U
#elif (SPI_PRESCALER == 2U)
  #define SPI_PRIVATE_SPR_BITS  0x02U
  #define SPI_PRIVATE_SPI2X     0U
#elif (SPI_PRESCALER == 3U)
  #define SPI_PRIVATE_SPR_BITS  0x03U
  #define SPI_PRIVATE_SPI2X     0U
#elif (SPI_PRESCALER == 4U)
  #define SPI_PRIVATE_SPR_BITS  0x00U
  #define SPI_PRIVATE_SPI2X     1U
#elif (SPI_PRESCALER == 5U)
  #define SPI_PRIVATE_SPR_BITS  0x01U
  #define SPI_PRIVATE_SPI2X     1U
#elif (SPI_PRESCALER == 6U)
  #define SPI_PRIVATE_SPR_BITS  0x02U
  #define SPI_PRIVATE_SPI2X     1U
#else   /* 7 */
  #define SPI_PRIVATE_SPR_BITS  0x03U
  #define SPI_PRIVATE_SPI2X     1U
#endif

/* -----------------------------------------------------------------------
 * Build the SPCR initialisation byte
 *
 *   Bit 7 : SPIE – interrupt enable (from SPI_INTERRUPT_ENABLE)
 *   Bit 6 : SPE  – SPI enable (always 1 after init)
 *   Bit 5 : DORD – data order  (from SPI_DATA_ORDER)
 *   Bit 4 : MSTR – master/slave (from SPI_MODE)
 *   Bit 3 : CPOL – clock polarity (from SPI_CPOL)
 *   Bit 2 : CPHA – clock phase   (from SPI_CPHA)
 *   Bit 1 : SPR1 – prescaler MSB
 *   Bit 0 : SPR0 – prescaler LSB
 * --------------------------------------------------------------------- */
#define SPI_PRIVATE_SPCR_INIT  \
    ((u8)(((SPI_INTERRUPT_ENABLE & 0x01U) << 7U) | \
          (1U                             << 6U) | \
          ((SPI_DATA_ORDER        & 0x01U) << 5U) | \
          ((SPI_MODE              & 0x01U) << 4U) | \
          ((SPI_CPOL              & 0x01U) << 3U) | \
          ((SPI_CPHA              & 0x01U) << 2U) | \
          (SPI_PRIVATE_SPR_BITS  & 0x03U)))

#endif /* MSPI_PRIVATE_H_ */
