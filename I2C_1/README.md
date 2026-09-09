# USART 1 — PC LED Control

AVR (ATmega32) lab: send a digit from the PC over **USART** to **toggle** the matching LED on **PORTA**.

## Behaviour

| Character sent | LED | Pin |
|----------------|-----|-----|
| `'1'` | LED 1 | PA0 |
| `'2'` | LED 2 | PA1 |
| `'3'` | LED 3 | PA2 |
| `'4'` | LED 4 | PA3 |
| `'5'` | LED 5 | PA4 |
| `'6'` | LED 6 | PA5 |
| `'7'` | LED 7 | PA6 |
| `'8'` | LED 8 | PA7 |

Any other character (including CR/LF) is ignored. A valid digit is echoed back to the terminal.

## Hardware

| Signal | AVR pin |
|--------|---------|
| USART RX | **PD0 (RXD)** |
| USART TX | **PD1 (TXD)** |
| LEDs 1–8 | **PA0–PA7** |

USART settings: **9600 baud, 8 data bits, no parity, 1 stop bit** (`8N1`), CPU **8 MHz**.

### Proteus / kit

1. Connect a **Virtual Terminal** (or USB–UART) to `RXD`/`TXD`.
2. Terminal baud = **9600**.
3. Eight LEDs (with series resistors) from **PA0–PA7** to GND (active-high).

## Software

| Layer | Module | Purpose |
|-------|--------|---------|
| MCAL | `MUSART` | UART init, transmit, receive |
| MCAL | `MDIO` | GPIO |
| HAL | `HLED` | Eight LEDs on PORTA, including toggle |
| APP | `main.c` | Map `'1'`..`'8'` to LED 1..8 |

## Build

Open `SPI_1` in AVR Eclipse, **rebuild**, then load `Debug\SPI_1.hex`.
