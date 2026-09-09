/*
 * MSPI_interface.h
 *
 *  Created on: Sep 9, 2026
 *      Author: Anthony Gaius
 *
 *  Description:
 *      Public API for the ATmega32 SPI (Serial Peripheral Interface) MCAL driver.
 *
 *  ATmega32 SPI Pin Mapping (PORTB):
 *      PB7 – SCK   (Serial Clock)
 *      PB6 – MISO  (Master-In Slave-Out)
 *      PB5 – MOSI  (Master-Out Slave-In)
 *      PB4 – /SS   (Slave Select, active LOW)
 *
 *  Wiring (2 MCUs):
 *      MCU1 (Master)              MCU2 (Slave)
 *      PB7 (SCK)  ──────────────  PB7 (SCK)
 *      PB5 (MOSI) ──────────────  PB5 (MOSI)
 *      PB6 (MISO) ──────────────  PB6 (MISO)
 *      PB4 (/SS)  ──────────────  PB4 (/SS)   [driven LOW by master]
 *      GND        ──────────────  GND
 */

#ifndef MSPI_INTERFACE_H_
#define MSPI_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

/* -----------------------------------------------------------------------
 * SPI Mode (role) constants – used with SPI_MODE in MSPI_config.h
 * --------------------------------------------------------------------- */
#define SPI_MODE_SLAVE   0U
#define SPI_MODE_MASTER  1U

/* -----------------------------------------------------------------------
 * Data Order constants – used with SPI_DATA_ORDER in MSPI_config.h
 * --------------------------------------------------------------------- */
#define SPI_DATA_ORDER_MSB_FIRST  0U
#define SPI_DATA_ORDER_LSB_FIRST  1U

/* -----------------------------------------------------------------------
 * Error codes
 * --------------------------------------------------------------------- */
typedef enum {
    SPI_OK = 0,
    SPI_ERR_NULL_POINTER,
    SPI_ERR_TIMEOUT,
    SPI_ERR_WRITE_COLLISION
} SPI_ErrorStatus_t;

/* -----------------------------------------------------------------------
 * Driver API
 * --------------------------------------------------------------------- */

/**
 * @brief  Initialise the SPI peripheral.
 *
 *         Configures SPCR and SPSR according to the values in
 *         MSPI_config.h and sets the required PORTB pin directions:
 *           Master: SCK/MOSI/SS  → output, MISO → input
 *           Slave : SCK/MOSI/SS  → input,  MISO → output
 *
 *         Call this function once during system startup, before any
 *         transmit/receive calls.
 */
void SPI_voidInit(void);

/**
 * @brief  Transmit one byte and simultaneously receive one byte (full-duplex).
 *
 *         In Master mode the function:
 *           1. Writes Copy_u8TxData to SPDR (starts SCK generation).
 *           2. Polls SPIF until transfer is complete (or timeout).
 *           3. Reads the received byte from SPDR into *Copy_pu8RxData.
 *
 *         In Slave mode the function:
 *           1. Writes Copy_u8TxData to SPDR (pre-loads the shift register).
 *           2. Waits for the master to clock the transfer.
 *           3. Reads the received byte from SPDR into *Copy_pu8RxData.
 *
 * @param  Copy_u8TxData    Byte to transmit.
 * @param  Copy_pu8RxData   Pointer to store the received byte.
 * @return SPI_OK           Transfer completed successfully.
 *         SPI_ERR_NULL_POINTER  Copy_pu8RxData is NULL.
 *         SPI_ERR_TIMEOUT       SPIF was never set within the timeout.
 *         SPI_ERR_WRITE_COLLISION WCOL flag was set (data written while busy).
 */
SPI_ErrorStatus_t SPI_enumTransceive(u8 Copy_u8TxData, u8 *Copy_pu8RxData);

/**
 * @brief  Transmit one byte (discard received byte).
 *
 * @param  Copy_u8TxData   Byte to send.
 * @return SPI_OK / SPI_ERR_TIMEOUT / SPI_ERR_WRITE_COLLISION
 */
SPI_ErrorStatus_t SPI_enumTransmit(u8 Copy_u8TxData);

/**
 * @brief  Receive one byte (transmit a dummy 0x00 to generate clock in master mode).
 *
 * @param  Copy_pu8RxData   Pointer to store the received byte.
 * @return SPI_OK / SPI_ERR_NULL_POINTER / SPI_ERR_TIMEOUT
 */
SPI_ErrorStatus_t SPI_enumReceive(u8 *Copy_pu8RxData);

/**
 * @brief  Wait for SPIF (transfer-complete) WITHOUT writing to SPDR.
 *
 *         This is the correct function to call in Slave mode after pre-loading
 *         SPDR with an echo value.  Unlike SPI_enumReceive() which calls
 *         SPI_enumTransceive() and immediately writes SPDR = 0x00 (destroying
 *         any pre-loaded echo), this function ONLY polls SPIF and then reads
 *         SPDR to clear the flag.
 *
 *         Typical slave echo usage:
 *         @code
 *         SPI_enumWaitForTransfer(&rx);   // Tx1: receive value (SPDR = pre-loaded 0x00)
 *         SPDR = rx;                       // pre-load echo for Tx2
 *         // ... process ...
 *         SPI_enumWaitForTransfer(&dummy); // Tx2: echo shifts out, discard master dummy
 *         SPDR = 0x00U;                   // reset for next Tx1
 *         @endcode
 *
 * @param  Copy_pu8RxData   Pointer to store the received byte.
 * @return SPI_OK / SPI_ERR_NULL_POINTER / SPI_ERR_TIMEOUT
 */
SPI_ErrorStatus_t SPI_enumWaitForTransfer(u8 *Copy_pu8RxData);

/**
 * @brief  Transmit a buffer of bytes, discarding all received data.
 *
 * @param  Copy_pu8TxBuffer   Pointer to bytes to send.
 * @param  Copy_u8Length      Number of bytes.
 * @return SPI_OK on full success, or first error encountered.
 */
SPI_ErrorStatus_t SPI_enumTransmitBuffer(const u8 *Copy_pu8TxBuffer, u8 Copy_u8Length);

/**
 * @brief  Receive a buffer of bytes (transmit dummy 0x00 for each byte).
 *
 * @param  Copy_pu8RxBuffer   Pointer to receive buffer.
 * @param  Copy_u8Length      Number of bytes to receive.
 * @return SPI_OK on full success, or first error encountered.
 */
SPI_ErrorStatus_t SPI_enumReceiveBuffer(u8 *Copy_pu8RxBuffer, u8 Copy_u8Length);

/**
 * @brief  Register a callback to be called from the SPI transfer-complete ISR.
 *
 *         Only relevant when SPI_INTERRUPT_ENABLE == 1U in MSPI_config.h.
 *         Calling this with a NULL pointer clears the callback.
 *
 * @param  Copy_pvCallback   Function pointer (void fn(void)).
 * @return SPI_OK always.
 */
SPI_ErrorStatus_t SPI_enumSetCallback(void (*Copy_pvCallback)(void));

/**
 * @brief  Assert (drive LOW) the SS pin from software.
 *
 *         In most designs the SS line is toggled manually in application
 *         code before/after a transaction.  This helper simply clears PB4.
 *         Only meaningful when operating as SPI Master.
 */
void SPI_voidSelectSlave(void);

/**
 * @brief  De-assert (drive HIGH) the SS pin from software.
 */
void SPI_voidDeselectSlave(void);

#endif /* MSPI_INTERFACE_H_ */
