# MOTOR_2 — 2-Relay H-Bridge DC Motor Control

AVR (ATmega32) project that controls DC motor direction using a **2-relay H-bridge** with **transistor-isolated** relay drivers.

## Hardware Overview

### Relay H-Bridge

| K1 (CW) | K2 (CCW) | Motor |
|---------|----------|-------|
| OFF | OFF | Stop |
| ON | OFF | Clockwise |
| OFF | ON | Counter-clockwise |
| ON | ON | **Forbidden** (short-circuit) |

### Pin Map

| AVR Pin | Function |
|---------|----------|
| PD0 | NPN Q1 → Relay K1 (CW) |
| PD1 | NPN Q2 → Relay K2 (CCW) |
| PB6 | OFF button (active low, internal pull-up) |
| PB7 | MODE button (active low, internal pull-up) |
| PC0 | Power LED |
| PC1 | Direction LED (ON = CCW) |

### Transistor Relay Driver (per channel)

```
AVR Pin --[1 kΩ]-- Base (NPN)
                   Emitter -> GND
                   Collector -> Relay Coil -> Motor Supply (+)
                   Flyback diode across coil (cathode to +)
```

### Buttons

Wire each button between the pin and **GND**. The firmware uses **internal pull-up** (`HPB_PULL_UP`).

## Software Architecture

| Layer | Module | Purpose |
|-------|--------|---------|
| MCAL | `MTIMER` | 1 ms system tick (Timer0 CTC) |
| MCAL | `MDIO` | GPIO |
| HAL | `HDCMOTOR` | Non-blocking relay H-bridge driver |
| HAL | `HPB` | Debounced push buttons with edge detection |
| HAL | `HLED` | Status LEDs |
| APP | `main.c` | Button logic and status display |

## Operation

- **PB6 (OFF):** Toggle motor power. Turning off resets direction to CW.
- **PB7 (MODE):** Toggle direction **only while motor is ON**.
- Direction changes use a **50 ms relay dead-time** (non-blocking).
- **PC0** lights when motor is on.
- **PC1** lights when running CCW.

## Configuration

| File | Setting |
|------|---------|
| `CONFIG/APP/APP_config.h` | Buttons, LED/LCD enable flags |
| `CONFIG/DCMOTOR/HDCMOTOR_config.h` | Relay pins, switch delay |
| `CONFIG/PB/HPB_config.h` | Debounce sample count (default: 3) |
| `CONFIG/DIO/MDIO_config.h` | Default port directions |

### Optional LCD

Set `APP_USE_LCD` to `1` in `APP_config.h`. The LCD uses Port A (control) and Port B (data bus), so buttons cannot share Port B in that configuration.

## Build

Open the project in AVR Eclipse / Atmel Studio and build the **Debug** configuration. The 1 ms timer ISR must be linked (`MCAL/TIMER/MTIMER_program.c`).

## API Summary

### Motor (non-blocking)

```c
HDCMOTOR_enumInit(&motor);
HDCMOTOR_enumRequestRun(&motor, HDCMOTOR_CW);
HDCMOTOR_voidUpdate(&motor);   // call every main-loop iteration
HDCMOTOR_enumRequestStop(&motor);
```

### Buttons

```c
HPB_enumInit(&btn, port, pin, HPB_PULL_UP);
HPB_voidUpdate(&btn);
HPB_enumGetEdge(&btn, &edge);  // HPB_EDGE_PRESSED / RELEASED / NONE
```
