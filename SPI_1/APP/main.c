/*
 * main.c
 *
 *  Assignment: SPI Temperature-Controlled DC Motor
 *  -----------------------------------------------
 *  MCU 1 (SPI Master):
 *      - Reads temperature from an LM35 sensor via ADC (PA0).
 *      - Sends the temperature value (1 byte, °C) to MCU 2 over SPI
 *        every ~500 ms.
 *
 *  MCU 2 (SPI Slave):
 *      - Waits to receive the temperature byte via SPI.
 *      - If temperature > 26 °C  → turns the DC Motor ON  (CW direction).
 *      - If temperature ≤ 26 °C  → turns the DC Motor OFF (stop).
 *
 *  ─── SPI Wiring (ATmega32 ↔ ATmega32) ───────────────────────────────
 *
 *   MCU 1 (Master)   Wire   MCU 2 (Slave)
 *   PB7  (SCK)  ──────────  PB7  (SCK)
 *   PB5  (MOSI) ──────────  PB5  (MOSI)
 *   PB6  (MISO) ──────────  PB6  (MISO)
 *   PB4  (/SS)  ──────────  PB4  (/SS)
 *   GND         ──────────  GND
 *
 *  ─── LM35 Wiring (MCU 1 only) ───────────────────────────────────────
 *   LM35 VCC  → +5V
 *   LM35 GND  → GND
 *   LM35 VOUT → PA0 (ADC Channel 0)
 *
 *  ─── DC Motor Wiring (MCU 2 only) ───────────────────────────────────
 *   See HDCMOTOR_config.h:
 *       PD0 → Relay K1 (CW)
 *       PD1 → Relay K2 (CCW)
 *
 *  ─── How to select which MCU firmware to build ───────────────────────
 *   Change the #define below:
 *       #define ACTIVE_MCU  MCU_1_MASTER   ← flash onto MCU 1
 *       #define ACTIVE_MCU  MCU_2_SLAVE    ← flash onto MCU 2
 */

#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "../LIB/BIT_MATH.h"
#include "../LIB/REGISTERS.h"
#include "../LIB/STD_TYPES.h"

#include "../MCAL/DIO/MDIO_interface.h"
#include "../MCAL/ADC/MADC_interface.h"
#include "../MCAL/TIMER/MTIMER_interface.h"
#include "../MCAL/SPI/MSPI_interface.h"

#include "../HAL/LM35/HLM35_interface.h"
#include "../HAL/DCMOTOR/HDCMOTOR_interface.h"

/* ─── MCU role selection ─────────────────────────────────────────────── */
#define MCU_1_MASTER  1
#define MCU_2_SLAVE   2

/* >>> CHANGE THIS LINE to select which firmware to compile <<< */
#define ACTIVE_MCU   MCU_1_MASTER
/* ─────────────────────────────────────────────────────────────────────── */

/* Temperature threshold in degrees Celsius */
#define TEMP_THRESHOLD_C   26U

/* How often MCU 1 samples the temperature and sends it (milliseconds) */
#define SAMPLE_INTERVAL_MS 500U

/* =========================================================================
 * MCU 1 – SPI MASTER: reads LM35, sends temperature over SPI
 * ======================================================================= */
#if ACTIVE_MCU == MCU_1_MASTER

/*
 * MSPI_config.h for MCU 1 must have:
 *     SPI_MODE  1U   (MASTER)
 *
 * MADC_config.h must have:
 *     ADC_REF_VOLTAGE  1U   (AVCC = 5 V)
 *     ADC_PRESCALER    5U   (÷64 → 125 kHz ADC clock @ 8 MHz)
 */

int main(void) {

    u8 local_u8TempC     = 0U;
    u32 local_u32LastMs  = 0U;
    u32 local_u32NowMs   = 0U;

    /* ------------------------------------------------------------------
     * Peripheral initialisation
     * ------------------------------------------------------------------ */

    /* Timer must be first – MADC_Init uses MTIMER_voidDelayMs internally */
    MTIMER_voidInit();

    /* DIO – configure port directions via DIO_voidInit() (uses DIO_config.h)
     * SPI pins (PORTB) are configured inside SPI_voidInit(), so no conflict. */
    DIO_voidInit();

    /* ADC – required by the LM35 HAL */
    ADC_voidInit();

    /* SPI Master – PB7(SCK), PB5(MOSI), PB4(/SS) → output; PB6(MISO) → input */
    SPI_voidInit();

    /* Give the slave MCU a moment to complete its own SPI_voidInit() */
    MTIMER_voidDelayMs(100U);

    local_u32LastMs = MTIMER_u32GetMillis();

    /* ------------------------------------------------------------------
     * Main loop
     * ------------------------------------------------------------------ */
    while (1) {

        local_u32NowMs = MTIMER_u32GetMillis();

        /* Sample and transmit every SAMPLE_INTERVAL_MS milliseconds */
        if ((local_u32NowMs - local_u32LastMs) >= SAMPLE_INTERVAL_MS) {
            local_u32LastMs = local_u32NowMs;

            /* Read temperature from LM35 */
            if (LM35_enumReadTemperature(&local_u8TempC) == LM35_OK) {

                /* Assert /SS to select the slave */
                SPI_voidSelectSlave();

                /* Send the temperature byte */
                SPI_enumTransmit(local_u8TempC);

                /* De-assert /SS – transaction complete */
                SPI_voidDeselectSlave();
            }
        }
    }

    return 0;
}

/* =========================================================================
 * MCU 2 – SPI SLAVE: receives temperature, controls DC Motor
 * ======================================================================= */
#elif ACTIVE_MCU == MCU_2_SLAVE

/*
 * MSPI_config.h for MCU 2 must have:
 *     SPI_MODE  0U   (SLAVE)
 *
 * HDCMOTOR_config.h:
 *     HDCMOTOR_RELAY_PORT     DIO_PORTD
 *     HDCMOTOR_RELAY_CW_PIN   DIO_PIN0   (PD0 → relay / H-bridge IN1)
 *     HDCMOTOR_RELAY_CCW_PIN  DIO_PIN1   (PD1 → relay / H-bridge IN2)
 */

int main(void) {

    u8             local_u8ReceivedTemp  = 0U;
    SPI_ErrorStatus_t local_enumSpiErr  = SPI_OK;

    /* DC Motor descriptor – pins from HDCMOTOR_config.h */
    HDCMOTOR_t local_stMotor = {
        .Port            = HDCMOTOR_RELAY_PORT,
        .RelayCwPin      = HDCMOTOR_RELAY_CW_PIN,
        .RelayCcwPin     = HDCMOTOR_RELAY_CCW_PIN,
        .ActiveDirection = HDCMOTOR_STOP,
        .RequestedDirection = HDCMOTOR_STOP,
        .State           = HDCMOTOR_STATE_STOPPED,
        .SwitchDeadlineMs = 0U
    };

    /* ------------------------------------------------------------------
     * Peripheral initialisation
     * ------------------------------------------------------------------ */

    /* Timer first (required by HDCMOTOR_voidUpdate timing) */
    MTIMER_voidInit();

    /* DIO port directions */
    DIO_voidInit();

    /* DC Motor – sets relay pins as outputs and de-energises both relays */
    HDCMOTOR_enumInit(&local_stMotor);

    /* SPI Slave – PB6(MISO) → output; PB7/PB5/PB4 → inputs */
    SPI_voidInit();

    /* ------------------------------------------------------------------
     * Main loop
     * ------------------------------------------------------------------ */
    while (1) {

        /*
         * In slave mode SPI_enumReceive() waits until the master
         * drives a full 8-bit clock cycle (/SS must be LOW on the slave).
         * When SPIF is set, SPDR holds the byte sent by the master.
         */
        local_enumSpiErr = SPI_enumReceive(&local_u8ReceivedTemp);

        if (local_enumSpiErr == SPI_OK) {

            if (local_u8ReceivedTemp > TEMP_THRESHOLD_C) {
                /* Temperature exceeded threshold – run motor CW */
                HDCMOTOR_voidRun(&local_stMotor, HDCMOTOR_CW);
            } else {
                /* Temperature at or below threshold – stop motor */
                HDCMOTOR_voidStop(&local_stMotor);
            }
        }

        /* Keep the motor state machine updated (handles relay switch delay) */
        HDCMOTOR_voidUpdate(&local_stMotor);
    }

    return 0;
}

#endif /* ACTIVE_MCU */
