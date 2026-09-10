/*
 * MSPI_program.c
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      ATmega32 SPI (Serial Peripheral Interface) MCAL driver implementation.
 *
 *  Supported features:
 *      - Master & Slave mode
 *      - Configurable clock polarity (CPOL) and phase (CPHA)  → SPI Modes 0–3
 *      - Configurable clock prescaler (/2 … /128)
 *      - MSB-first or LSB-first data order
 *      - Full-duplex transceive (TX + RX simultaneously)
 *      - Separate Transmit-only and Receive-only helpers
 *      - Buffer transmit / receive
 *      - Optional interrupt-driven mode with user callback
 *      - Timeout guard to prevent infinite blocking
 *      - Software SS (slave-select) assertion helpers
 */

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"
#include "../../LIB/REGISTERS.h"

#include <avr/interrupt.h>

#include "../../HW/SPI/MSPI_private.h"
#include "MSPI_interface.h"

/* -----------------------------------------------------------------------
 * Private module state
 * --------------------------------------------------------------------- */
static void (*s_pvTransferCallback)(void) = NULL;

/* -----------------------------------------------------------------------
 * SPI_voidInit
 * --------------------------------------------------------------------- */
void SPI_voidInit(void) {

    /*
     * Configure PORTB pin directions.
     *
     *   PB7 – SCK   Master: OUT  Slave: IN
     *   PB6 – MISO  Master: IN   Slave: OUT
     *   PB5 – MOSI  Master: OUT  Slave: IN
     *   PB4 – /SS   Master: OUT  Slave: IN  (must be kept HIGH or
     *                                         as an input for slave
     *                                         to work correctly)
     */
#if (SPI_MODE == SPI_MODE_MASTER)
    /* SCK, MOSI, SS → output */
    SET_BIT(DDRB, PB7);   /* SCK  */
    SET_BIT(DDRB, PB5);   /* MOSI */
    SET_BIT(DDRB, PB4);   /* /SS  – driven by software */
    /* MISO → input (master reads from slave) */
    CLR_BIT(DDRB, PB6);
    /* Pull MISO high through internal pull-up (optional) */
    SET_BIT(PORTB, PB6);
    /* De-assert SS (HIGH = not selected) */
    SET_BIT(PORTB, PB4);
#else   /* SLAVE */
    /* MISO → output (slave sends to master) */
    SET_BIT(DDRB, PB6);
    /* SCK, MOSI, SS → inputs */
    CLR_BIT(DDRB, PB7);   /* SCK  */
    CLR_BIT(DDRB, PB5);   /* MOSI */
    CLR_BIT(DDRB, PB4);   /* /SS  */
#endif

    /* ---------------------------------------------------------------
     * Configure SPCR (SPI Control Register)
     * --------------------------------------------------------------- */
    SPCR = SPI_PRIVATE_SPCR_INIT;

    /* ---------------------------------------------------------------
     * Configure SPSR (SPI Status Register) – SPI2X double-speed bit
     * --------------------------------------------------------------- */
#if (SPI_PRIVATE_SPI2X == 1U)
    SET_BIT(SPSR, SPI2X);
#else
    CLR_BIT(SPSR, SPI2X);
#endif

    /* Clear SPIF by reading SPSR then SPDR (if set from a previous use) */
    (void)SPSR;
    (void)SPDR;
}

/* -----------------------------------------------------------------------
 * SPI_enumTransceive  – core full-duplex byte transfer
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumTransceive(u8 Copy_u8TxData, u8 *Copy_pu8RxData) {
    u32 local_u32Timeout;

    if (Copy_pu8RxData == NULL) {
        return SPI_ERR_NULL_POINTER;
    }

    /* Check for a write-collision from a previous transfer */
    if (GET_BIT(SPSR, WCOL) == 1U) {
        /* Clear WCOL: read SPSR then write SPDR */
        (void)SPSR;
        SPDR = Copy_u8TxData;
        /* Return error; caller should retry */
        return SPI_ERR_WRITE_COLLISION;
    }

    /* Write data to the shift register; in master mode this starts SCK. */
    SPDR = Copy_u8TxData;

    /* Poll SPIF (transfer-complete flag) */
    local_u32Timeout = 0UL;
    while (GET_BIT(SPSR, SPIF) == 0U) {
        local_u32Timeout++;
        if (local_u32Timeout >= SPI_TIMEOUT_COUNT) {
            return SPI_ERR_TIMEOUT;
        }
    }

    /*
     * Reading SPSR (which we already checked) and then SPDR clears SPIF
     * automatically per the ATmega32 datasheet.
     */
    *Copy_pu8RxData = SPDR;

    return SPI_OK;
}

/* -----------------------------------------------------------------------
 * SPI_enumTransmit  – send one byte, discard received byte
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumTransmit(u8 Copy_u8TxData) {
    u8 local_u8Dummy = 0U;
    return SPI_enumTransceive(Copy_u8TxData, &local_u8Dummy);
}

/* -----------------------------------------------------------------------
 * SPI_enumReceive  – receive one byte (sends dummy 0x00)
 * NOTE: This function writes 0x00 to SPDR before polling SPIF.
 *       Do NOT use it in Slave mode after pre-loading SPDR with an echo
 *       value – use SPI_enumWaitForTransfer() instead.
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumReceive(u8 *Copy_pu8RxData) {
    if (Copy_pu8RxData == NULL) {
        return SPI_ERR_NULL_POINTER;
    }
    return SPI_enumTransceive(0x00U, Copy_pu8RxData);
}

/* -----------------------------------------------------------------------
 * SPI_enumWaitForTransfer  – poll SPIF WITHOUT writing SPDR
 *
 * This is the correct function for Slave echo/loopback scenarios:
 *
 *   Problem with SPI_enumReceive() in slave echo mode:
 *     SPI_enumReceive() → SPI_enumTransceive(0x00, &rx) → SPDR = 0x00
 *     That write DESTROYS any pre-loaded echo value in SPDR before the
 *     master's next /SS assertion.  The master then clocks out 0x00 (or
 *     garbage) instead of the echo.
 *
 *   This function NEVER touches SPDR.  It only:
 *     1. Polls SPIF until set (or timeout)
 *     2. Reads SPDR to clear SPIF
 *   So any value pre-loaded into SPDR stays there until the master
 *   starts the next SCK cycle and shifts it out.
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumWaitForTransfer(u8 *Copy_pu8RxData) {
    u32 local_u32Timeout;

    if (Copy_pu8RxData == NULL) {
        return SPI_ERR_NULL_POINTER;
    }

    /*
     * Poll SPIF without touching SPDR.
     * Any value previously written to SPDR is preserved and will be
     * shifted out on MISO the next time the master drives SCK.
     */
    local_u32Timeout = 0UL;
    while (GET_BIT(SPSR, SPIF) == 0U) {
        local_u32Timeout++;
        if (local_u32Timeout >= SPI_TIMEOUT_COUNT) {
            return SPI_ERR_TIMEOUT;
        }
    }

    /*
     * Per the ATmega32 datasheet, SPIF is cleared by:
     *   1. Reading SPSR with SPIF set  (the while-loop did this)
     *   2. Then accessing SPDR         (this line)
     */
    *Copy_pu8RxData = SPDR;

    return SPI_OK;
}

/* -----------------------------------------------------------------------
 * SPI_enumTransmitBuffer  – send an array of bytes
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumTransmitBuffer(const u8 *Copy_pu8TxBuffer, u8 Copy_u8Length) {
    SPI_ErrorStatus_t Local_enumErr = SPI_OK;
    u8 local_u8Index;

    if (Copy_pu8TxBuffer == NULL) {
        return SPI_ERR_NULL_POINTER;
    }

    for (local_u8Index = 0U; local_u8Index < Copy_u8Length; local_u8Index++) {
        Local_enumErr = SPI_enumTransmit(Copy_pu8TxBuffer[local_u8Index]);
        if (Local_enumErr != SPI_OK) {
            break;  /* Return on first error */
        }
    }

    return Local_enumErr;
}

/* -----------------------------------------------------------------------
 * SPI_enumReceiveBuffer  – receive an array of bytes
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumReceiveBuffer(u8 *Copy_pu8RxBuffer, u8 Copy_u8Length) {
    SPI_ErrorStatus_t Local_enumErr = SPI_OK;
    u8 local_u8Index;

    if (Copy_pu8RxBuffer == NULL) {
        return SPI_ERR_NULL_POINTER;
    }

    for (local_u8Index = 0U; local_u8Index < Copy_u8Length; local_u8Index++) {
        Local_enumErr = SPI_enumReceive(&Copy_pu8RxBuffer[local_u8Index]);
        if (Local_enumErr != SPI_OK) {
            break;
        }
    }

    return Local_enumErr;
}

/* -----------------------------------------------------------------------
 * SPI_enumSetCallback  – register ISR callback
 * --------------------------------------------------------------------- */
SPI_ErrorStatus_t SPI_enumSetCallback(void (*Copy_pvCallback)(void)) {
    s_pvTransferCallback = Copy_pvCallback;
    return SPI_OK;
}

/* -----------------------------------------------------------------------
 * SPI_voidSelectSlave / SPI_voidDeselectSlave
 * --------------------------------------------------------------------- */
void SPI_voidSelectSlave(void) {
    CLR_BIT(PORTB, PB4);   /* Assert  /SS = LOW */
}

void SPI_voidDeselectSlave(void) {
    SET_BIT(PORTB, PB4);   /* De-assert /SS = HIGH */
}

/* -----------------------------------------------------------------------
 * SPI Transfer-Complete ISR  (compiled only when interrupt mode is on)
 * --------------------------------------------------------------------- */
#if (SPI_INTERRUPT_ENABLE == 1U)
ISR(SPI_STC_vect) {
    if (s_pvTransferCallback != NULL) {
        s_pvTransferCallback();
    }
}
#endif
