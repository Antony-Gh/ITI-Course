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

- **Microcontroller**: ATmega32A (Running at 8 MHz).
- **Display**: 16x2 Character LCD (Standard GPIO, 4-bit mode).
- **Input (Digital)**: 4x4 Matrix Keypad.
- **Input (Analog)**: 10 kΩ Potentiometer (for frequency sweeps).
- **PWM Filter (DAC)**:
  - **Option 1 (Basic)**: 1x 4.7 kΩ Resistor, 1x 10 nF Ceramic Capacitor
  - **Option 2 (Laboratory Grade)**: LM358 Dual Op-Amp configured as a 2nd-order or 4th-order Sallen-Key Active Low-Pass Filter (using 1.5 kΩ resistors and 10 nF capacitors).

---

## 🔌 Pin Mapping & Schematic

| Component | ATmega32A Pin | Notes |
| :--- | :--- | :--- |
| **Potentiometer** | `PA0 (ADC0)` | Connect center wiper to PA0. Outer pins to 5V and GND. |
| **Keypad Rows** | `PB0 - PB3` | Row 0 = PB0, Row 3 = PB3 |
| **Keypad Cols** | `PB4 - PB7` | Col 0 = PB4, Col 3 = PB7 |
| **LCD Control** | `PC0 - PC2` | RS = PC0, RW = PC1, EN = PC2 |
| **LCD Data** | `PC4 - PC7` | D4 = PC4, D5 = PC5, D6 = PC6, D7 = PC7 |
| **Analog Output** | `PD5 (OC1A)` | Connect to the RC/Sallen-Key Low-Pass Filter. Used for Sine, Triangle, Sawtooth, Saddle. |
| **Square/Sync** | `PD4` | Pure digital output. Bypasses the filter entirely for razor-sharp square waves! |

### The "Split-Output" Architecture

Because analog low-pass filters fundamentally destroy the sharp edges of square waves, this generator uses a split-output hardware architecture:

1. **The Analog Path (`PD5`)**: The high-frequency 31.25 kHz PWM is output here. Connect this to your analog filter (e.g., an LM358 Sallen-Key). Probe the output of your op-amp when you want to view Sine, Triangle, Sawtooth, or Saddle waves.
2. **The Digital Path (`PD4`)**: This pin acts as a dedicated TTL Sync/Square output. Connect your oscilloscope probe directly to this pin. When you select the Square Wave, this pin instantly jumps from 0V to 5V with zero slipping or RC-filter delays!

---

## 🎮 Operating Instructions

- **Dynamic Sweeping**: Turn the Potentiometer to smoothly sweep the frequency!
- **Exact Frequency Override**: Type the exact frequency you want using the numeric keys (e.g., `1 0 0 0`) and press **`=` (Enter)**. This will instantly override the potentiometer until you turn the pot again.
- **Change Multiplier**:
  - Press **`+`** for the **x1** range (1 - 1024 Hz).
  - Press **`-`** for the **x10** range (10 - 10,240 Hz).
  - Press **`*`** for the **x100** range (100 - 102,400 Hz).
- **Change Waveform**: Press **`/`** to cycle through the available wave shapes (Sine → Triangle → Square → Sawtooth → Saddle).
- **Clear Input**: Made a typo while typing a frequency? Press **`C`** to clear the screen before hitting Enter.
- **Auto-Save**: The system automatically saves your state to the internal EEPROM whenever you hit Enter, change the multiplier, or change the waveform.

---

## 🧠 Software Architecture

## 🧠 Software Architecture

The codebase utilizes two independent hardware timers to ensure flawless timing:

1. **Timer1 (The DDS Engine & PWM DAC)**: Runs in 8-bit Fast PWM mode, overflowing precisely 31,250 times per second (Fs = F_CPU / 256). The Timer1 Overflow Interrupt (ISR) advances the 32-bit Phase Accumulator, looks up the current wave sample from Flash memory, and instantly updates the `OCR1A` duty cycle. Because the sample update happens exactly when the PWM cycle restarts, the output is 100% synchronized and jitter-free.
2. **Timer2 (The System Tick)**: Runs in CTC mode, triggering every 1 millisecond. It maintains a system clock (`s_u32Millis`) that allows `main.c` to poll the ADC, keypad, and LCD in a completely non-blocking manner.

---

## 🏗️ Compilation & Build

This project is built using the standard AVR-GCC toolchain (compatible with the IMT SDK / Eclipse CDT environment).

1. Ensure the microcontroller is set to **ATmega32** or **ATmega32A**.
2. Ensure `F_CPU` is defined as `8000000UL` (8 MHz internal/external).
3. Flash the resulting `.hex` file to your real hardware or load it directly into Proteus 8 Professional for simulation.
