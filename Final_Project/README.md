# ATmega32A DDS Function Generator

A professional-grade, minimal hardware Direct Digital Synthesis (DDS) function generator built for the ATmega32A microcontroller.

This project was developed for the ITI Embedded Systems course. It replaces complex parallel DAC ICs with a high-frequency PWM DAC and RC-filter architecture. It features a completely non-blocking UI, an analog potentiometer for ultra-smooth frequency sweeps, and a 4x4 keypad for exact frequency overrides!

---

## 🌟 Features
- **Direct Digital Synthesis (DDS)**: Generates highly accurate, stable, and jitter-free analog waveforms.
- **5 Waveform Types**: Sine, Triangle, Square, Sawtooth, and a custom "Saddle" wave.
- **Dynamic Frequency Sweeping**: An analog Potentiometer provides real-time, jitter-free frequency control.
- **Exact Numeric Override**: Use the 4x4 keypad to instantly type and lock in an exact frequency.
- **Wide Frequency Range**: 1 Hz to 100,000 Hz (using hardware multipliers).
- **Non-Blocking Architecture**: The user interface runs purely on a 1ms System Tick, meaning UI updates never interrupt the waveform.
- **Auto-Save Memory**: Remembers your frequency, waveform, and multiplier settings using the internal ATmega32A EEPROM.
- **Single-Pin Output**: Generates pure analog waves using just a single microcontroller pin (`PD5`) and an RC Low-Pass filter.

---

## 🛠️ Hardware Requirements

- **Microcontroller**: ATmega32A (Running at 16 MHz via external crystal).
- **Display**: 16x2 Character LCD (Standard GPIO, 4-bit mode).
- **Input (Digital)**: 4x4 Matrix Keypad.
- **Input (Analog)**: 10 kΩ Potentiometer (for frequency sweeps).
- **PWM Filter (DAC)**: 
  - 1x 4.7 kΩ Resistor
  - 1x 10 nF Ceramic Capacitor

---

## 🔌 Pin Mapping & Schematic

| Component | ATmega32A Pin | Notes |
| :--- | :--- | :--- |
| **Potentiometer** | `PA0 (ADC0)` | Connect center wiper to PA0. Outer pins to 5V and GND. |
| **Keypad Rows** | `PB0 - PB3` | Row 0 = PB0, Row 3 = PB3 |
| **Keypad Cols** | `PB4 - PB7` | Col 0 = PB4, Col 3 = PB7 |
| **LCD Control** | `PC0 - PC2` | RS = PC0, RW = PC1, EN = PC2 |
| **LCD Data** | `PC4 - PC7` | D4 = PC4, D5 = PC5, D6 = PC6, D7 = PC7 |
| **PWM Output** | `PD5 (OC1A)` | Connect to the RC Low-Pass Filter. |

### The RC Low-Pass Filter (The PWM DAC)
To convert the high-frequency digital PWM pulses on `PD5` into a smooth analog wave:
1. Connect the **4.7 kΩ resistor** in series with `PD5`.
2. Connect the **10 nF capacitor** from the other side of the resistor to **Ground**.
3. The junction between the resistor and the capacitor is your analog signal. Connect your oscilloscope probe here!

---

## 🎮 Operating Instructions

* **Dynamic Sweeping**: Turn the Potentiometer to smoothly sweep the frequency!
* **Exact Frequency Override**: Type the exact frequency you want using the numeric keys (e.g., `1 0 0 0`) and press **`=` (Enter)**. This will instantly override the potentiometer until you turn the pot again.
* **Change Multiplier**: 
  * Press **`+`** for the **x1** range (1 - 1024 Hz).
  * Press **`-`** for the **x10** range (10 - 10,240 Hz).
  * Press **`*`** for the **x100** range (100 - 102,400 Hz).
* **Change Waveform**: Press **`/`** to cycle through the available wave shapes (Sine → Triangle → Square → Sawtooth → Saddle).
* **Clear Input**: Made a typo while typing a frequency? Press **`C`** to clear the screen before hitting Enter.
* **Auto-Save**: The system automatically saves your state to the internal EEPROM whenever you hit Enter, change the multiplier, or change the waveform.

---

## 🧠 Software Architecture

The codebase utilizes three independent hardware timers to ensure perfect timing:

1. **Timer0 (The DDS Tick)**: Runs in CTC mode with no prescaler. It triggers an interrupt exactly 62,500 times per second. The ISR advances a 32-bit Phase Accumulator, looks up the current wave sample from a lookup table in Flash memory, and instantly writes it to Timer1.
2. **Timer1 (The PWM DAC)**: Runs in 8-bit Fast PWM mode. It operates at 62.5 kHz to match the DDS tick. It constantly outputs the varying duty cycle on `PD5`.
3. **Timer2 (The System Tick)**: Runs in CTC mode, triggering every 1 millisecond. It maintains a system clock (`s_u32Millis`) that allows `main.c` to handle the keypad and LCD in a completely non-blocking manner.

---

## 🏗️ Compilation & Build

This project is built using the standard AVR-GCC toolchain (compatible with the IMT SDK / Eclipse CDT environment).

1. Ensure the microcontroller is set to **ATmega32** or **ATmega32A**.
2. Ensure `F_CPU` is defined as `16000000UL` (16 MHz).
3. Flash the resulting `.hex` file to your microcontroller or load it directly into Proteus 8 Professional for simulation.
