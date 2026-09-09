/*
 * main.c
 *
 *  Created on: Aug 31, 2026
 *      Author: Anthony Gaius
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/STD_TYPES.h"

/*
 * BUILD_CONFIG.h is the ONLY file you need to edit between builds.
 * It defines ACTIVE_MCU and derives SPI_MODE from it.
 * Both this file and MCAL/SPI/MSPI_program.c include it, so the SPI
 * hardware is always configured to match the application role.
 */
#include "../LIB/BUILD_CONFIG.h"

#include "../MCAL/ADC/MADC_interface.h"
#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/SPI/MSPI_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"

#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"
#include "../HAL/LCD/HLCD_interface.h"
#include "../HAL/LM35/HLM35_interface.h"

/* Temperature threshold in degrees Celsius */
#define TEMP_THRESHOLD_C 26U

/* How often MCU 1 samples the temperature and sends it (milliseconds) */
#define SAMPLE_INTERVAL_MS 100U

/* =========================================================================
 * MCU 1 SPI MASTER: reads LM35, sends temperature over SPI
 * ======================================================================= */
#if ACTIVE_MCU == MCU_1_MASTER

/*
 * MSPI_config.h for MCU 1 must have:
 *     SPI_MODE  1U   (MASTER)
 *
 * MADC_config.h must have:
 *     ADC_REF_VOLTAGE  1U   (AVCC = 5 V)
 *     ADC_PRESCALER    5U   (64 125 kHz ADC clock @ 8 MHz)
 */

int main(void) {

  u8 local_u8TempC = 0U;    /* temperature read from LM35     */
  u8 local_u8EchoBack = 0U; /* echo received back from slave  */
  u32 local_u32LastMs = 0U;
  u32 local_u32NowMs = 0U;

  /* ------------------------------------------------------------------
   * Peripheral initialisation
   * ------------------------------------------------------------------ */

  /* Timer must be first – MADC_Init uses MTIMER_voidDelayMs internally */
  MTIMER_voidInit();

  /* DIO – configure port directions (SPI pins handled inside SPI_voidInit) */
  DIO_voidInit();

  /* LCD */
  HLCD_voidInit();
  HLCD_voidGoToXY(0, 1);
  HLCD_voidSendString("SPI Temp Demo");

  /* ADC – required by the LM35 HAL */
  ADC_voidInit();

  /* SPI Master – PB7(SCK), PB5(MOSI), PB4(/SS) -> output; PB6(MISO) -> input */
  SPI_voidInit();

  /* Give the slave MCU a moment to complete its own SPI_voidInit() */
  MTIMER_voidDelayMs(200U);

  local_u32LastMs = MTIMER_u32GetMillis();

  /* ------------------------------------------------------------------
   * Main loop
   * ------------------------------------------------------------------
   *
   * Single full-duplex transaction per cycle:
   *
   *   Master MOSI ──> temp_N          (temperature just read from LM35)
   *   Master MISO <── temp_(N-1)      (echo the slave sent from prev cycle)
   *
   * Both directions happen simultaneously on the same 8 SCK pulses.
   * There is 1-cycle latency on the echo (echo shows the previous reading),
   * which is perfectly acceptable for a slowly-changing temperature.
   * ------------------------------------------------------------------ */
  while (1) {

    local_u32NowMs = MTIMER_u32GetMillis();

    /* Sample and transmit every SAMPLE_INTERVAL_MS milliseconds */
    if ((local_u32NowMs - local_u32LastMs) >= SAMPLE_INTERVAL_MS) {
      local_u32LastMs = local_u32NowMs;

      /* Read temperature from LM35 */
      if (LM35_enumReadTemperature(&local_u8TempC) == LM35_OK) {

        /* Single full-duplex transaction:
         *   MOSI sends temp_N to slave
         *   MISO receives temp_(N-1) echo from slave (pre-loaded last cycle) */
        SPI_voidSelectSlave();
        MTIMER_voidDelayMs(5U); // give slave time to prepare
        SPI_enumTransceive(local_u8TempC, &local_u8EchoBack);
        SPI_voidDeselectSlave();

        /* Display on LCD */
        HLCD_voidGoToXY(0, 1);
        HLCD_voidSendString("Sent:");
        HLCD_voidSendNumber(local_u8TempC);
        HLCD_voidSendString("C    ");

        HLCD_voidGoToXY(1, 1);
        HLCD_voidSendString("Echo:");
        HLCD_voidSendNumber(local_u8EchoBack);
        HLCD_voidSendString("C  ");
      }
    }
  }

  return 0;
}

/* =========================================================================
 * MCU 2 SPI SLAVE: receives temperature, controls DC Motor
 * ======================================================================= */
#elif ACTIVE_MCU == MCU_2_SLAVE

int main(void) {

  u8 local_u8ReceivedTemp = 0U;

  /* DC Motor descriptor – pins from HDCMOTOR_config.h */
  HDCMOTOR_t local_stMotor = {.Port = HDCMOTOR_RELAY_PORT,
                              .RelayCwPin = HDCMOTOR_RELAY_CW_PIN,
                              .RelayCcwPin = HDCMOTOR_RELAY_CCW_PIN,
                              .ActiveDirection = HDCMOTOR_STOP,
                              .RequestedDirection = HDCMOTOR_STOP,
                              .State = HDCMOTOR_STATE_STOPPED,
                              .SwitchDeadlineMs = 0U};

  /* ------------------------------------------------------------------
   * Peripheral initialisation
   * ------------------------------------------------------------------ */

  MTIMER_voidInit();
  DIO_voidInit();
  HDCMOTOR_enumInit(&local_stMotor);

  /* SPI Slave – PB6(MISO) -> output; PB7/PB5/PB4 -> inputs
   * Pre-load SPDR = 0x00 so MISO is defined on the very first transaction. */
  SPI_voidInit();
  SPDR = 0x00U;

  /* ------------------------------------------------------------------
   * Main loop
   * ------------------------------------------------------------------
   *
   * Single full-duplex transaction per cycle:
   *
   *   Slave MOSI <── temp_N            (received from master)
   *   Slave MISO ──> temp_(N-1)        (echo pre-loaded from previous cycle)
   *
   * Protocol:
   *   1. SPI_enumWaitForTransfer() waits for SPIF WITHOUT writing SPDR.
   *      This is critical: the previous cycle's echo stays in SPDR so the
   *      hardware SPI shifts it out on MISO during this exact transaction.
   *   2. IMMEDIATELY after receiving, write SPDR = received value so it
   *      is ready to be echoed in the NEXT transaction.
   *   3. Control the motor.
   *   4. Loop — no second transaction needed.
   * ------------------------------------------------------------------ */
  while (1) {

    /* Wait for master to clock 8 bits.
     * SPDR contains the echo from the previous cycle and is shifted out
     * simultaneously as we receive the new temperature on MOSI.        */
    (void)SPI_enumWaitForTransfer(&local_u8ReceivedTemp);

    /* Pre-load echo for the NEXT cycle immediately.
     * Must happen before the master's next /SS assertion (~250 ms away). */
    // SPDR = 0x55; // always echo 0x55
    SPDR = local_u8ReceivedTemp;

    /* Control DC Motor based on received temperature */
    if (local_u8ReceivedTemp > TEMP_THRESHOLD_C) {
      HDCMOTOR_voidRun(&local_stMotor, HDCMOTOR_CW);
    } else {
      HDCMOTOR_voidStop(&local_stMotor);
    }

    HDCMOTOR_voidUpdate(&local_stMotor);
  }

  return 0;
}

#endif /* ACTIVE_MCU */
