# MOTOR_VECTOR — EXTI DC Motor Toggle

AVR (ATmega32) project that toggles a DC motor **ON/OFF** using a push button on an **EXTI** pin (external interrupt).

## Assignment

Use a push button to toggle a DC motor on and off utilizing an EXTI pin from the microcontroller.

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
| **PD2** | **INT0 — toggle button** (active low, internal pull-up) |
| PC0 | Power LED (ON when motor runs) |
| PC1 | Stop LED (ON when motor is off) |

### Toggle Button (EXTI)

- Wire the button between **PD2 (INT0)** and **GND**.
- Firmware configures **falling-edge** interrupt with internal pull-up.
- Each press toggles motor power (OFF → ON → OFF …).
- Motor runs **clockwise** when ON.

### Transistor Relay Driver (per channel)

```
AVR Pin --[1 kΩ]-- Base (NPN)
                   Emitter -> GND
                   Collector -> Relay Coil -> Motor Supply (+)
                   Flyback diode across coil (cathode to +)
```

## Software Architecture

| Layer | Module | Purpose |
|-------|--------|---------|
| MCAL | `MTIMER` | 1 ms system tick (Timer0 CTC) |
| MCAL | `MDIO` | GPIO |
| MCAL | `MEXTI` | External interrupts (INT0/INT1/INT2) |
| HAL | `HDCMOTOR` | Non-blocking relay H-bridge driver |
| HAL | `HLED` | Status LED |
| APP | `main.c` | EXTI callback + motor toggle logic |

## Operation Flow

```
Button press (PD2 ↓)  →  INT0 ISR sets toggle flag
                       →  Main loop debounces (waits for release)
                       →  Toggles motor ON/OFF
                       →  PC0 LED reflects state
```

## Configuration

| File | Setting |
|------|---------|
| `CONFIG/APP/APP_config.h` | Toggle button pin, EXTI ID, LED enable |
| `CONFIG/EXTI/MEXTI_config.h` | Interrupt sense, enable flags |
| `CONFIG/DCMOTOR/HDCMOTOR_config.h` | Relay pins, switch delay |

## Build

Open the project in AVR Eclipse / Atmel Studio and build the **Debug** configuration.

## API Summary

### EXTI

```c
EXTI_voidInit();
EXTI_u8SetCallback(EXTI_u8_INT0, APP_voidToggleIsr);
EXTI_voidEnableGlobal();
```

### Motor (non-blocking)

```c
HDCMOTOR_enumInit(&motor);
HDCMOTOR_enumRequestRun(&motor, HDCMOTOR_CW);
HDCMOTOR_voidUpdate(&motor);   // call every main-loop iteration
HDCMOTOR_enumRequestStop(&motor);
```
