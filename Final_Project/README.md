# ATmega32A DDS Function Generator

A professional-grade, ultra-minimal hardware Direct Digital Synthesis (DDS) function generator built from scratch for the ATmega32A microcontroller.

This project was developed for the ITI Embedded Systems course. It replaces complex and expensive parallel DAC ICs (like the DAC0808) with a high-frequency PWM DAC and RC-filter architecture, while offloading the entire user interface (LCD, Keypad, and EEPROM) onto a single I2C bus to save GPIO pins.

---

## 🌟 Features

- **Direct Digital Synthesis (DDS)**: Generates highly accurate, stable, and jitter-free analog waveforms.
- **5 Waveform Types**: Sine, Triangle, Square, Sawtooth, and a custom "Saddle" wave.
- **Wide Frequency Range**: 1 Hz to 20,000 Hz.
- **Non-Blocking Architecture**: The user interface runs purely on a 1ms System Tick, meaning UI updates will never interrupt the high-speed waveform generation.
- **Auto-Save Memory**: Remembers your frequency and waveform settings across reboots using an external I2C 24C64 EEPROM.
- **Single-Pin Output**: Generates pure analog waves using just a single microcontroller pin (PD5) and an RC Low-Pass filter.

---

## 🛠️ Hardware Requirements

- **Microcontroller**: ATmega32A (Running at 16 MHz via external crystal).
- **Display**: 16x2 Character LCD with a PCF8574 I2C Backpack (Address `0x27`).
- **Input**: Matrix Keypad connected via two PCF8574 I2C Expanders (Addresses `0x21` & `0x22`).
- **Memory**: 24C64 I2C EEPROM (Address `0x50`).
- **PWM Filter (DAC)**:
  - 1x 4.7 kΩ Resistor
  - 1x 10 nF Ceramic Capacitor

---

## 🔌 Pin Mapping & Schematic

| Component | ATmega32A Pin | Notes |
| :--- | :--- | :--- |
| **I2C SCL** | `PC0` | Connect to SCL on all I2C devices. Requires 4.7kΩ Pull-up to 5V. |
| **I2C SDA** | `PC1` | Connect to SDA on all I2C devices. Requires 4.7kΩ Pull-up to 5V. |
| **PWM Output** | `PD5 (OC1A)` | Connect to the RC Low-Pass Filter. |

### The RC Low-Pass Filter (The PWM DAC)

To convert the high-frequency digital PWM pulses on `PD5` into a smooth analog wave:

1. Connect the **4.7 kΩ resistor** in series with `PD5`.
2. Connect the **10 nF capacitor** from the other side of the resistor to **Ground**.
3. The junction between the resistor and the capacitor is your analog signal. Connect your oscilloscope probe here!

---

## 🧠 Software Architecture

The codebase strictly adheres to a layered architecture (MCAL → HAL → SERVICES → APP) and utilizes three independent hardware timers to ensure perfect timing:

1. **Timer0 (The DDS Tick)**: Runs in CTC mode with no prescaler. It triggers an interrupt exactly 62,500 times per second. The ISR advances a 32-bit Phase Accumulator, looks up the current wave sample from a lookup table in Flash memory, and instantly writes it to Timer1.
2. **Timer1 (The PWM DAC)**: Runs in 8-bit Fast PWM mode. It operates at 62.5 kHz to match the DDS tick. It constantly outputs the varying duty cycle on `PD5`.
3. **Timer2 (The System Tick)**: Runs in CTC mode, triggering every 1 millisecond. It maintains a system clock (`s_u32Millis`) that allows `main.c` to handle the keypad and LCD in a completely non-blocking manner.

---

## 🎮 Operating Instructions

- **Change Frequency**: Type the exact frequency you want using the numeric keys (e.g., `1 0 0 0`) and press **`E` (Enter)** to apply it instantly.
- **Step Frequency**: Press **`U` (Up)** or **`D` (Down)** to step the frequency. The step size scales automatically based on your current range (steps of 1 Hz, 10 Hz, 100 Hz, or 1 kHz).
- **Change Waveform**: Press **`W`** to cycle through the available wave shapes (Sine → Triangle → Square → Sawtooth → Saddle).
- **Clear Input**: Made a typo? Press **`C`** to clear the screen before hitting Enter.
- **Force Save**: Press **`S`** to force-save your current setup to the EEPROM (Note: The system automatically saves whenever you hit Enter or change the waveform).

---

## 🏗️ Compilation & Build

This project is built using the standard AVR-GCC toolchain (compatible with the IMT SDK / Eclipse CDT environment).

1. Ensure the microcontroller is set to **ATmega32** or **ATmega32A**.
2. Ensure `F_CPU` is defined as `16000000UL` (16 MHz).
3. Flash the resulting `.hex` file to your microcontroller or load it directly into Proteus 8 Professional for simulation.
