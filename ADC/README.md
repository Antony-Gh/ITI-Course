# ADC — Potentiometer Test

AVR (ATmega32) project that reads a **potentiometer** through the **MADC** driver and displays the raw ADC value and percentage on a **16×2 LCD**.

## Hardware Overview

### Potentiometer (ADC Input)

| Connection | AVR Pin |
|------------|---------|
| Wiper (middle) | **PA0 (ADC0)** |
| One leg | **VCC (5 V)** |
| Other leg | **GND** |

Turning the pot changes the voltage on PA0 from 0 V to 5 V. The ADC converts this to a 10-bit value (0–1023).

### LCD (16×2, 8-bit interface)

| AVR Pin | Function |
|---------|----------|
| PC0 | LCD RS |
| PC1 | LCD RW |
| PC2 | LCD EN |
| PB0–PB7 | LCD Data (D0–D7) |

PORTA is kept free for the ADC input. LCD control was moved to **PORTC** so the pot can use the standard **PA0 / ADC0** pin used in most Proteus schematics.

### Proteus — Required Connections

If you see `[AVR AD CONVERTER] Reference value = 0` in the simulation log, fix the **ATmega32 power/reference pins**:

| Pin | Connect to |
|-----|------------|
| **VCC** | +5 V |
| **AVCC** (pin 30) | **+5 V** (required for ADC) |
| **AREF** (pin 32) | **100 nF capacitor to GND** |
| **GND** | Ground |

Without **AVCC** tied to +5 V, Proteus reports reference = 0 and the ADC reading sticks at maximum.

Also verify in Proteus:

1. Pot **wiper → PA0** (not another pin)
2. LCD **RS/RW/EN → PC0/PC1/PC2** (update wires if they were on PA0–PA2)
3. LCD **D0–D7 → PB0–PB7**

### LCD Display Format

```
R:50.0kOhm
ADC:512 50%
```

- **R** — wiper-to-GND resistance (0.0–100.0 kΩ for a 100 kΩ pot)
- **ADC** — 10-bit reading (0–1023)
- **%** — position as percentage (0–100 %)

## Software Architecture

| Layer | Module | Purpose |
|-------|--------|---------|
| MCAL | `MADC` | 10-bit ADC driver |
| MCAL | `MTIMER` | 1 ms tick for sample interval |
| MCAL | `MDIO` | GPIO (LCD + pot pin) |
| HAL | `HLCD` | 16×2 character LCD |
| APP | `main.c` | Periodic ADC read + LCD update |

## Operation Flow

```
Every 200 ms  →  ADC_enumReadChannel(ADC0)
              →  Convert to percentage
              →  Update LCD (raw value + %)
```

## Configuration

| File | Setting |
|------|---------|
| `CONFIG/APP/APP_config.h` | Pot channel (ADC0), sample interval |
| `CONFIG/ADC/MADC_config.h` | Reference voltage (AVCC), prescaler |
| `CONFIG/DIO/MDIO_config.h` | Pin directions |
| `CONFIG/LCD/HLCD_config.h` | LCD control and data ports |

## Build

Open the project in AVR Eclipse / Atmel Studio, **rebuild**, then load `Debug\ADC.hex` in Proteus.

## API Summary

### ADC

```c
ADC_voidInit();
ADC_enumReadChannel(ADC_CHANNEL_0, &adcValue);  // blocking read
```
