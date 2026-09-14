# Phase 1 — ATmega32A Function / Waveform Generator Engineering Design

## 1  System Requirements

| Requirement | Target |
| :--- | :--- |
| Waveform types | Sine, Triangle, Square, Sawtooth, Saddle |
| Frequency range | 1 Hz – 20 kHz (analog), up to ~100 kHz (digital square) |
| Frequency resolution | ≤ 1 Hz at low end |
| Output amplitude | 0 – 4.5 Vpp (single-supply, adjustable) |
| DAC resolution | 8 bit (256 levels) |
| User controls | 3 push-buttons (waveform, freq-up, freq-down) + 1 potentiometer (amplitude) |
| Display | 16×2 character LCD |
| MCU | ATmega32A @ 16 MHz external crystal |
| Simulation | Proteus 8.17 SP5 — all components from built-in library |
| Power | Single +5 V supply |

---

## 2  Architecture Comparison

### 2.1  Option A — ATmega32A + R-2R / DAC0808

The MCU outputs 8-bit samples to a parallel DAC on every timer interrupt.
The DDS algorithm (phase accumulator + LUT) runs inside the MCU.

### 2.2  Option B — ATmega32A + AD9833 DDS IC

The MCU sends frequency/waveform commands over SPI to a dedicated DDS chip.
The AD9833 contains its own phase accumulator, sine ROM, and DAC.

### 2.3  Engineering Comparison

| Feature | Option A (R-2R / DAC0808) | Option B (AD9833) |
| :--- | :--- | :--- |
| **Hardware complexity** | Low — 16 resistors + 1 op-amp | Low — 1 IC + decoupling |
| **Software complexity** | Medium — DDS engine + ISR + LUTs | Low — SPI register writes |
| **Max analog frequency** | ~20 kHz (limited by ISR overhead) | ~12.5 MHz (hardware DDS) |
| **Frequency resolution** | ~0.93 Hz (32-bit accumulator @ 62.5 kHz Fs) | ~0.1 Hz (28-bit, 25 MHz MCLK) |
| **Sine quality** | Good (256-point LUT, 8-bit) | Excellent (built-in ROM, 10-bit DAC) |
| **Triangle generation** | Easy (LUT or math) | Built-in (register bit) |
| **Square generation** | Easy (MSB of accumulator, or OC pin) | Built-in, but needs external comparator for clean edges |
| **Sawtooth generation** | Easy (upper byte of accumulator) | ❌ Not natively supported |
| **Saddle / custom wave** | Easy (custom LUT) | ❌ Not supported at all |
| **Proteus simulation** | ✅ All components in library (DAC0808, resistors, LM358) | ❌ **No AD9833 model in Proteus** |
| **Cost (BOM)** | ~$2 (resistors + op-amp) | ~$8–12 (AD9833 module) |
| **Educational value** | **Very High** — exposes DDS, DAC, analog, ISR design | Medium — mostly SPI configuration |
| **Difficulty** | Medium | Low (but debugging without simulation is hard) |
| **Expandability** | Add more LUTs, PWM modes, dual-channel | Limited to sine/triangle/square |

### 2.4  Recommendation

> [!IMPORTANT]
> **Option A (ATmega32A + DAC0808 + R-2R) is the clear winner for this project.**

Key reasons:

1. **Proteus compatibility** — The AD9833 has **no simulation model** in Proteus 8.17. This is a project-killer since Proteus simulation is a hard requirement.
2. **Custom waveforms** — Sawtooth and saddle wave are trivial with LUTs but impossible with the AD9833.
3. **Educational value** — The entire DDS pipeline is visible and teachable: phase accumulator → LUT → DAC → filter → output.
4. **ITI curriculum alignment** — Uses concepts directly from your course: timers, interrupts, ADC, DIO, PROGMEM, LCD.

The 20 kHz analog frequency ceiling is perfectly acceptable for a lab-grade function generator demonstration.

> [!NOTE]
> We will **also** provide a clean hardware square-wave output using Timer1 OC1A (up to ~8 MHz), completely independent of the DAC path. This gives the project a "bonus" high-frequency digital output.

---

## 3  System Block Diagram

```
               ┌──────────────────────────────────────────────────┐
               │                  ATmega32A  (16 MHz)             │
               │                                                  │
  [BTN_WAVE]──►│ PD2 (INT0)                         PA0..PA7 ────┼──► 8-bit DAC
  [BTN_UP]────►│ PD3                                              │    (DAC0808)
  [BTN_DOWN]──►│ PD4                                              │       │
               │                                                  │       ▼
  [POT_AMP]──►│ ADC7 (PA7 shared — see §4)         OC1A (PD5) ──┼──► HW Square Out
               │                                                  │
  [LCD_RS]◄───│ PC0                                              │
  [LCD_RW]◄───│ PC1                                              │
  [LCD_EN]◄───│ PC2                                              │
  [LCD_D4]◄───│ PC4                                              │
  [LCD_D5]◄───│ PC5                                              │
  [LCD_D6]◄───│ PC6                                              │
  [LCD_D7]◄───│ PC7                                              │
               └──────────────────────────────────────────────────┘
```

```
  DAC0808 Output ──► Iout-to-Voltage (R) ──► LPF (RC or Active) ──► Buffer (LM358) ──► WAVE OUT
```

---

## 4  Pin Allocation

> [!WARNING]
> **Critical change from existing codebase**: The current LCD configuration uses **PORTA** for 8-bit data. PORTA is the only full 8-bit port that also serves the ADC. For the waveform generator, we need PORTA dedicated to the DAC (all 8 pins driving the DAC0808 inputs). The LCD must be reconfigured to **4-bit mode on PORTC**.

### 4.1  Final Pin Map

| Port | Pin | Function | Direction |
| :--- | :--- | :--- | :--- |
| **PA0** | PA0 | DAC D0 (LSB) | Output |
| **PA1** | PA1 | DAC D1 | Output |
| **PA2** | PA2 | DAC D2 | Output |
| **PA3** | PA3 | DAC D3 | Output |
| **PA4** | PA4 | DAC D4 | Output |
| **PA5** | PA5 | DAC D5 | Output |
| **PA6** | PA6 | DAC D6 | Output |
| **PA7** | PA7 | DAC D7 (MSB) | Output |
| **PB0** | PB0 | *Reserved (T0 ext clock)* | — |
| **PB1** | PB1 | *Reserved (T1 ext clock)* | — |
| **PB2** | PB2 | *Reserved (INT2)* | — |
| **PB3** | PB3 | OC0 — *available for PWM amplitude* | Output |
| **PB4** | PB4 | *Free / SS* | — |
| **PB5** | PB5 | *Free / MOSI* | — |
| **PB6** | PB6 | *Free / MISO* | — |
| **PB7** | PB7 | *Free / SCK* | — |
| **PC0** | PC0 | LCD RS | Output |
| **PC1** | PC1 | LCD RW | Output |
| **PC2** | PC2 | LCD EN | Output |
| **PC3** | PC3 | *Free* | — |
| **PC4** | PC4 | LCD D4 | Output |
| **PC5** | PC5 | LCD D5 | Output |
| **PC6** | PC6 | LCD D6 | Output |
| **PC7** | PC7 | LCD D7 | Output |
| **PD0** | PD0 | *Free / RXD* | — |
| **PD1** | PD1 | *Free / TXD* | — |
| **PD2** | PD2 | BTN_WAVE (INT0, pull-up) | Input |
| **PD3** | PD3 | BTN_FREQ_UP (pull-up) | Input |
| **PD4** | PD4 | BTN_FREQ_DOWN (pull-up) | Input |
| **PD5** | PD5 | OC1A — HW square-wave output | Output |
| **PD6** | PD6 | *Free* | — |
| **PD7** | PD7 | *Free* | — |
| **AREF** | — | 5 V via 100 nF decoupling | — |

### 4.2  Amplitude Control — Design Decision

Since PORTA is fully consumed by the DAC, we **cannot** use the ATmega32A ADC for a potentiometer-based amplitude reading (ADC channels share PORTA pins). Two practical options:

> [!IMPORTANT]
> **Amplitude control options — please choose:**
>
> **Option 1 (Recommended):** Use a **passive analog potentiometer** in the output stage (between the filter output and the final buffer). No ADC needed. The LCD shows "AMP: POT" or a fixed label. This is simpler, lower-risk, and avoids port conflicts.
>
> **Option 2:** Multiplex PORTA between DAC output and ADC reads. During a brief window (e.g., every 100 ms), disable the DAC ISR, reconfigure PA7 as ADC input, read the pot, reconfigure as output, resume. This adds complexity, introduces a brief glitch, and is fragile.
>
> **Option 3:** Use a digital potentiometer (MCP41010) controlled via SPI on PORTB. Clean but adds an extra IC.

**My recommendation is Option 1** — a simple mechanical potentiometer wired as a voltage divider in the analog output path. This is how real bench function generators handle amplitude — with an analog attenuator, not digital.

---

## 5  DDS Algorithm — Detailed Mathematics

### 5.1  Phase Accumulator

The core of Direct Digital Synthesis is a **phase accumulator** — a free-running counter that wraps around at 2^N.

```c
/* 32-bit phase accumulator */
static volatile u32 g_u32PhaseAcc = 0;
static volatile u32 g_u32PhaseInc = 0;

/* Inside Timer0 CTC ISR (called at Fs) */
g_u32PhaseAcc += g_u32PhaseInc;
```

The **upper 8 bits** of the 32-bit accumulator form the LUT index:

```c
u8 lutIndex = (u8)(g_u32PhaseAcc >> 24);
```

### 5.2  Frequency Equation

$$f_{out} = \frac{\text{phaseIncrement} \times f_s}{2^{32}}$$

Solving for the phase increment:

$$\text{phaseIncrement} = \frac{f_{out} \times 2^{32}}{f_s}$$

### 5.3  Frequency Resolution

The minimum nonzero frequency step is when `phaseIncrement = 1`:

$$\Delta f = \frac{f_s}{2^{32}}$$

With $f_s = 62{,}500$ Hz (see §6):

$$\Delta f = \frac{62{,}500}{4{,}294{,}967{,}296} \approx 0.0000145 \text{ Hz}$$

This is **far better** than needed. Even at $f_s = 62.5$ kHz we get sub-millihertz resolution.

### 5.4  Phase Increment Calculation (Integer-Only)

To avoid floating-point in the ISR, we precompute `phaseIncrement` when the user changes frequency:

```c
/* Called from main loop when user changes frequency */
/* f_desired is in Hz (integer) */
void DDS_voidSetFrequency(u32 Copy_u32FreqHz)
{
    /* phaseInc = (f_desired * 2^32) / Fs                */
    /* To avoid overflow: (f_desired * (2^32 / Fs))      */
    /* 2^32 / 62500 = 68719.4... ≈ 68719 (truncation)    */
    /* Better: use 64-bit intermediate                    */
    u64 temp = (u64)Copy_u32FreqHz * 4294967296ULL;
    g_u32PhaseInc = (u32)(temp / DDS_SAMPLE_RATE);
}
```

### 5.5  Maximum Output Frequency (Nyquist)

By Nyquist, the maximum representable frequency is $f_s / 2 = 31{,}250$ Hz.

In practice, to maintain waveform quality (≥10 samples/cycle), we limit to:

$$f_{max,\text{practical}} \approx \frac{f_s}{10} = 6{,}250 \text{ Hz}$$

For reasonable sine quality (≥5 samples/cycle):

$$f_{max,\text{sine}} \approx \frac{f_s}{5} = 12{,}500 \text{ Hz}$$

**Design target: 20 kHz** if we push the sample rate to 100 kHz (see §6 analysis).

---

## 6  Timer Configuration & Sample Rate

### 6.1  Timer Selection

| Timer | Role |
| :--- | :--- |
| **Timer0** | Waveform sample generation — CTC mode, generates ISR at fixed $f_s$ |
| **Timer1** | Hardware square-wave output on OC1A (PD5) — CTC toggle mode |
| **Timer2** | System tick (1 ms) for LCD, buttons, debouncing — existing driver |

### 6.2  Timer0 — Waveform Sample Clock

**Mode:** CTC (Clear Timer on Compare Match)  
**Prescaler:** 1 (no prescaling)  
**Clock:** 16 MHz

$$f_s = \frac{f_{CPU}}{N \times (\text{OCR0} + 1)}$$

| OCR0 | Prescaler N | $f_s$ | Period | Cycles per ISR |
| :--- | :--- | :--- | :--- | :--- |
| 255 | 1 | 62,500 Hz | 16.0 µs | 256 |
| 199 | 1 | 80,000 Hz | 12.5 µs | 200 |
| 159 | 1 | 100,000 Hz | 10.0 µs | 160 |
| 127 | 1 | 125,000 Hz | 8.0 µs | 128 |
| 79 | 1 | 200,000 Hz | 5.0 µs | 80 |

### 6.3  ISR Cycle Budget

The waveform ISR must complete within the inter-interrupt period. Let's estimate the ISR cost:

```
ISR entry (push regs, vector jump)       ~20 cycles
Load phaseAcc (32-bit)                   ~8 cycles
Add phaseInc (32-bit)                    ~8 cycles
Store phaseAcc                           ~8 cycles
Shift right 24 (extract index)          ~6 cycles
PROGMEM LUT read (pgm_read_byte)        ~6 cycles
Write to PORTA                          ~2 cycles
ISR exit (pop regs, reti)               ~16 cycles
─────────────────────────────────────────────────
Total estimate                          ~74 cycles
```

At 16 MHz, 74 cycles = **4.6 µs**.

| $f_s$ | Period | Available cycles | ISR cycles | CPU headroom |
| :--- | :--- | :--- | :--- | :--- |
| 62,500 Hz | 16.0 µs | 256 | ~74 | **71%** free ✅ |
| 100,000 Hz | 10.0 µs | 160 | ~74 | **54%** free ✅ |
| 125,000 Hz | 8.0 µs | 128 | ~74 | **42%** free ⚠️ |
| 200,000 Hz | 5.0 µs | 80 | ~74 | **8%** free ❌ |

### 6.4  Recommended Sample Rate

> [!TIP]
> **$f_s = 62{,}500$ Hz** (OCR0 = 255, prescaler = 1) is the recommended starting point.
>
> - Leaves 71% CPU for main-loop tasks (LCD, buttons, ADC)
> - Provides Nyquist limit of 31.25 kHz
> - Practical max output ≈ **10 kHz** for clean sine, **20 kHz** for acceptable sine
> - If testing shows margin, we can increase to 100 kHz (OCR0 = 159)

**Exact calculation:**

$$f_s = \frac{16{,}000{,}000}{1 \times (255 + 1)} = \frac{16{,}000{,}000}{256} = 62{,}500 \text{ Hz}$$

### 6.5  Timer1 — Hardware Square Wave

Timer1 in CTC mode with OC1A toggle generates a clean 50% duty-cycle square wave with **zero CPU overhead** and **zero jitter**:

$$f_{square} = \frac{f_{CPU}}{2 \times N \times (\text{OCR1A} + 1)}$$

With prescaler = 1 and OCR1A variable:

| OCR1A | $f_{square}$ |
| :--- | :--- |
| 0 | 8 MHz |
| 7 | 1 MHz |
| 79 | 100 kHz |
| 799 | 10 kHz |
| 7999 | 1 kHz |
| 15999 | 500 Hz |

This gives the project a **high-frequency digital square-wave output** that far exceeds the DAC path.

### 6.6  Timer2 — System Tick (Keep Existing)

Timer2 remains in CTC mode at 8 MHz/64 with OCR2 = 124 → 1 ms tick.

> [!NOTE]
> We need to update `F_CPU` from `8000000UL` to `16000000UL` throughout the codebase for the function generator build, and recalculate Timer2 accordingly: OCR2 = (16000000 / 64 / 1000) - 1 = **249**.

---

## 7  Waveform LUT Design

### 7.1  Sine Wave (256-byte LUT, stored in Flash)

$$y[n] = 128 + 127 \times \sin\left(\frac{2\pi n}{256}\right), \quad n = 0,1,...,255$$

This maps one full sine cycle into unsigned 8-bit values [1, 255] centered at 128.

```c
const u8 DDS_au8SineLUT[256] PROGMEM = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
    218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
    245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255,
    255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246,
    245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
    218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
    128, 124, 121, 118, 115, 112, 109, 106, 103, 100,  97,  93,  90,  88,  85,  82,
     79,  76,  73,  70,  67,  65,  62,  59,  57,  54,  52,  49,  47,  44,  42,  40,
     37,  35,  33,  31,  29,  27,  25,  23,  21,  20,  18,  17,  15,  14,  12,  11,
     10,   9,   7,   6,   5,   5,   4,   3,   2,   2,   1,   1,   1,   0,   0,   0,
      0,   0,   0,   0,   1,   1,   1,   2,   2,   3,   4,   5,   5,   6,   7,   9,
     10,  11,  12,  14,  15,  17,  18,  20,  21,  23,  25,  27,  29,  31,  33,  35,
     37,  40,  42,  44,  47,  49,  52,  54,  57,  59,  62,  65,  67,  70,  73,  76,
     79,  82,  85,  88,  90,  93,  97, 100, 103, 106, 109, 112, 115, 118, 121, 124
};
```

**Memory cost:** 256 bytes Flash.

### 7.2  Triangle Wave (Computed, No LUT Needed)

The triangle wave can be derived directly from the phase accumulator without a LUT:

```c
/* upper byte of phaseAcc */
u8 phase = (u8)(g_u32PhaseAcc >> 24);

/* Triangle: ramp up 0→255 for first half, ramp down 255→0 for second half */
u8 sample;
if (phase < 128) {
    sample = phase << 1;        /* 0→254 */
} else {
    sample = (255 - phase) << 1; /* 254→0 */
}
```

**Memory cost:** 0 bytes (computed in ~6 extra ISR cycles).

### 7.3  Square Wave (MSB of Accumulator, No LUT)

```c
u8 sample = (g_u32PhaseAcc & 0x80000000UL) ? 255 : 0;
```

**Memory cost:** 0 bytes, ~3 ISR cycles.

For the **hardware square wave** on OC1A, the timer directly toggles the pin — no ISR involvement at all.

### 7.4  Sawtooth Wave (Upper Byte of Accumulator, No LUT)

The sawtooth is simply the upper 8 bits of the phase accumulator:

```c
u8 sample = (u8)(g_u32PhaseAcc >> 24);
```

This naturally produces a rising ramp from 0→255 that wraps.

**Memory cost:** 0 bytes, ~3 ISR cycles.

### 7.5  Saddle Wave (256-byte LUT, stored in Flash)

The "saddle wave" is a modified sine formed by adding a third-harmonic component:

$$V(\theta) = A \left[ \sin(\theta) + k \sin(3\theta) \right]$$

**Choosing k:** Setting $k = 0.3$ produces a clearly visible "saddle" shape — the positive and negative peaks develop a flattened/indented top that looks like a saddle. The waveform has a characteristic double-hump near each peak.

**Why k = 0.3?**

- $k = 1/6 ≈ 0.167$: subtle effect, hard to distinguish from sine on a scope
- $k = 0.3$: clearly visible saddle indentation, still smooth
- $k = 0.5$: very pronounced, starts to look like a clipped wave

**Normalization:** The peak of $\sin(\theta) + 0.3\sin(3\theta)$ must be found numerically. The maximum occurs near $\theta \approx 1.09$ rad, with peak value ≈ 1.1835. So:

$$y[n] = 128 + \text{round}\left( \frac{127}{\max} \left[ \sin\left(\frac{2\pi n}{256}\right) + 0.3 \sin\left(\frac{6\pi n}{256}\right) \right] \right)$$

This LUT will be precomputed offline and stored in PROGMEM.

**Memory cost:** 256 bytes Flash.

### 7.6  Total Memory Usage

| Item | Flash | RAM |
| :--- | :--- | :--- |
| Sine LUT | 256 B | 0 |
| Saddle LUT | 256 B | 0 |
| Triangle | 0 (computed) | 0 |
| Square | 0 (computed) | 0 |
| Sawtooth | 0 (computed) | 0 |
| DDS variables | 0 | 12 B |
| **Total** | **512 B** | **12 B** |

ATmega32A has 32 KB Flash and 2 KB RAM → **1.6% Flash, 0.6% RAM** for waveform data. Excellent.

---

## 8  DAC Design — DAC0808

### 8.1  Why DAC0808 Over R-2R

| Factor | DAC0808 | Discrete R-2R |
| :--- | :--- | :--- |
| Proteus model | ✅ Built-in, accurate | ✅ Resistors available |
| Linearity | Guaranteed ±0.19% (datasheet) | Depends on resistor tolerance |
| Settling time | 150 ns typical | Depends on parasitic C |
| Component count | 1 IC + bias resistors | 16 precision resistors |
| Reliability | High | Tolerance-sensitive |
| Educational value | High — teaches DAC IC usage | High — teaches R-2R theory |
| Proteus simulation speed | Fast | Slightly faster |

**Recommendation:** Use the **DAC0808** IC for the primary design. It is available in the Proteus library and gives guaranteed linearity. We will still explain R-2R theory in the documentation.

### 8.2  DAC0808 Circuit Design

The DAC0808 is a current-output DAC. Its output is a current proportional to the digital input:

$$I_{out} = I_{REF} \times \frac{D}{256}$$

Where $D$ is the 8-bit digital code (0–255) and $I_{REF}$ is the reference current.

**Reference current setup:**

$$I_{REF} = \frac{V_{REF}}{R_{REF}}$$

With $V_{REF} = 5\text{V}$ and $R_{REF} = 5\text{k}\Omega$:

$$I_{REF} = \frac{5}{5000} = 1 \text{ mA}$$

**Full-scale output current:**

$$I_{FS} = 1\text{mA} \times \frac{255}{256} \approx 0.996 \text{ mA}$$

### 8.3  Current-to-Voltage Conversion

The DAC0808 outputs current. We convert to voltage using an op-amp in transimpedance configuration:

$$V_{out} = -I_{out} \times R_f$$

With $R_f = 5\text{k}\Omega$:

$$V_{out,max} = -0.996\text{mA} \times 5\text{k}\Omega \approx -4.98\text{V}$$

> [!NOTE]
> The negative output is resolved in the next stage. For single-supply operation, we use a different topology — see §9.

### 8.4  DAC0808 Connections (Proteus)

| DAC0808 Pin | Connection |
| :--- | :--- |
| A1 (pin 5, MSB) | PA7 |
| A2 (pin 6) | PA6 |
| A3 (pin 7) | PA5 |
| A4 (pin 8) | PA4 |
| A5 (pin 9) | PA3 |
| A6 (pin 10) | PA2 |
| A7 (pin 11) | PA1 |
| A8 (pin 12, LSB) | PA0 |
| VREF+ (pin 14) | +5V through R_REF = 5kΩ |
| VREF− (pin 15) | GND |
| VCC (pin 13) | +5V |
| VEE (pin 3) | −5V (or GND with adjusted circuit) |
| Iout (pin 4) | To op-amp inverting input |
| Iout_bar (pin 2) | GND |
| Comp (pin 16) | 0.1 µF to GND |

### 8.5  Quantization

With 8-bit resolution and 5V output range:

$$\text{LSB} = \frac{5\text{V}}{256} = 19.53 \text{ mV}$$

$$\text{Quantization error} = \pm \frac{\text{LSB}}{2} = \pm 9.77 \text{ mV}$$

This gives **48 dB** of signal-to-quantization-noise ratio (SQNR = 6.02 × N + 1.76 dB ≈ 49.9 dB). Adequate for a demonstration-grade function generator.

---

## 9  Analog Output Stage

### 9.1  Op-Amp Selection

| Parameter | LM358 | TL072 | Requirement |
| :--- | :--- | :--- | :--- |
| Supply | Single 3–32V ✅ | Dual ±5V to ±18V | Single +5V preferred |
| Input CM includes GND | ✅ Yes | ❌ No | Must include GND |
| Slew rate | 0.3 V/µs | 13 V/µs | ≥ 0.5 V/µs for 20 kHz |
| Bandwidth | 1.1 MHz | 3 MHz | ≥ 200 kHz |
| Output swing (5V supply) | 0 to ~3.5V | N/A (needs ±supply) | 0 to ≥3V |
| Proteus model | ✅ | ✅ | Must be available |
| Cost | ~$0.30 | ~$0.60 | Low |

**Slew rate check for 20 kHz sine:**

$$SR_{required} = 2\pi f V_{peak} = 2\pi \times 20{,}000 \times 2.25 = 0.283 \text{ V/µs}$$

The LM358's 0.3 V/µs is **marginally sufficient** at 20 kHz with 4.5 Vpp. Above 20 kHz, distortion increases.

> [!TIP]
> **Use the LM358** for single-supply simplicity. It works well up to ~15 kHz for undistorted sine waves. At 20 kHz there will be slight slew-rate limiting visible on the scope — this is a real-world engineering trade-off worth discussing in the project report.
>
> If dual supply (±12V) is acceptable, the TL072 would be superior. But for Proteus + single-supply simplicity, LM358 wins.

### 9.2  Single-Supply DAC0808 + LM358 Circuit

For single-supply operation with the DAC0808:

```
         +5V
          │
          R_REF = 5kΩ
          │
  VREF+ ──┤
          │
       DAC0808
          │
  Iout ───┬──── R_f = 5kΩ ────┐
          │                    │
          │    ┌───────────┐   │
          └───►│ LM358 (−) │───┘───► V_out (0 to ~4.9V)
               │           │
    +2.5V ───►│ LM358 (+) │
    (bias)     └───────────┘
```

Actually, for simpler Proteus simulation, we'll use a more straightforward approach:

**Simplified design:** Connect DAC0808 Iout through a resistor to the inverting input of the LM358, with VEE at −5V to allow the current output to work properly. In Proteus, we can use a ±5V supply.

> [!IMPORTANT]
> **Supply decision needed:** Proteus makes it easy to use ±5V. For real hardware, a single +5V USB supply would require a charge-pump IC (like ICL7660) to generate −5V. The DAC0808 datasheet recommends VEE = −5V to −15V for proper operation.
>
> **Recommendation:** Use **±5V** in both Proteus and hardware design. In Proteus, just use two DC voltage sources. For real hardware, use an ICL7660-based charge pump or a 7905 regulator if a ±12V bench supply is available.

### 9.3  Output Stage Chain

```
DAC0808 ──► I-to-V (LM358 #1, inverting) ──► Reconstruction LPF ──► Buffer (LM358 #2, follower) ──► POT (amplitude) ──► 100Ω series ──► OUTPUT BNC
```

The LM358 is a dual op-amp, so both stages fit in one 8-pin IC.

---

## 10  Reconstruction Filter

### 10.1  Why We Need It

The DAC output is a "staircase" waveform — it holds each sample value for one sample period ($1/f_s$). This produces spectral images at $f_s, 2f_s, 3f_s, ...$ which must be filtered out to produce a clean analog waveform.

### 10.2  Filter Design

**Type:** 1st-order passive RC low-pass filter (simple, sufficient for demonstration).

**Cutoff frequency:** Should be above the maximum desired output frequency but well below $f_s$:

$$f_{max,output} = 10{,}000 \text{ Hz (target)}$$
$$f_s = 62{,}500 \text{ Hz}$$

Choose $f_c \approx 15{,}000$ Hz — this passes all desired frequencies with minimal attenuation while attenuating the first image at 62.5 kHz by:

$$\text{Attenuation at } f_s = \frac{1}{\sqrt{1 + (f_s/f_c)^2}} = \frac{1}{\sqrt{1 + (62500/15000)^2}} = \frac{1}{\sqrt{1 + 17.36}} \approx 0.233 = -12.7 \text{ dB}$$

A 2nd-order filter would give -25.4 dB at $f_s$ — better, but for demonstration the 1st-order is adequate.

### 10.3  Component Values

For $f_c = 15{,}000$ Hz:

$$f_c = \frac{1}{2\pi R C}$$

Choose $R = 1\text{k}\Omega$:

$$C = \frac{1}{2\pi \times 1000 \times 15000} = \frac{1}{94{,}247{,}780} \approx 10.6 \text{ nF}$$

**Use standard value:** $C = 10 \text{ nF}$ (0.01 µF), giving:

$$f_c = \frac{1}{2\pi \times 1000 \times 10 \times 10^{-9}} = 15{,}915 \text{ Hz}$$

**Attenuation at 10 kHz (max desired signal):**

$$|H(10\text{kHz})| = \frac{1}{\sqrt{1 + (10000/15915)^2}} = \frac{1}{\sqrt{1.395}} = 0.847 = -1.44 \text{ dB}$$

This is acceptable — only 1.4 dB loss at the highest frequency.

### 10.4  Filter Trade-off Summary

```
Higher fc (e.g., 25 kHz):
  ✅ Less attenuation of desired signal
  ❌ More staircase visible on scope
  
Lower fc (e.g., 8 kHz):
  ✅ Cleaner waveform, less staircase
  ❌ More attenuation above 8 kHz, signal rolls off
  
Chosen fc ≈ 16 kHz:
  ✅ Good compromise
  ✅ Standard component values (1kΩ + 10nF)
```

---

## 11  User Interface Design

### 11.1  Controls

| Control | Connection | Function |
| :--- | :--- | :--- |
| BTN_WAVE (PD2) | Active-low, internal pull-up | Cycle: SINE → TRI → SQR → SAW → SADDLE → SINE... |
| BTN_FREQ_UP (PD3) | Active-low, internal pull-up | Increase frequency (step depends on range) |
| BTN_FREQ_DOWN (PD4) | Active-low, internal pull-up | Decrease frequency |
| POT_AMP | Analog pot in output stage | Adjust output amplitude (manual) |

### 11.2  Frequency Step Logic

| Current Frequency | Step Size |
| :--- | :--- |
| 1 – 99 Hz | 1 Hz |
| 100 – 999 Hz | 10 Hz |
| 1,000 – 9,999 Hz | 100 Hz |
| 10,000 – 20,000 Hz | 1,000 Hz |

This gives intuitive control across the full range.

### 11.3  LCD Display Format

```
┌────────────────┐
│WAVE: SINE      │
│FREQ: 1.00 kHz  │
└────────────────┘
```

For square wave (showing both DAC and hardware outputs):

```
┌────────────────┐
│WAVE: SQUARE    │
│FREQ: 1.00 kHz  │
└────────────────┘
```

### 11.4  LCD in 4-Bit Mode

Since PORTA is used for the DAC, the LCD must use **4-bit mode** on PORTC:

| LCD Signal | MCU Pin |
| :--- | :--- |
| RS | PC0 |
| RW | PC1 |
| EN | PC2 |
| D4 | PC4 |
| D5 | PC5 |
| D6 | PC6 |
| D7 | PC7 |

PC3 remains free. The existing LCD driver must be refactored for 4-bit mode.

---

## 12  Software Architecture

```
APP/
├── main.c                          ← Main loop, state machine
│
SERVICES/
├── DDS/
│   ├── DDS_interface.h             ← DDS engine API
│   ├── DDS_program.c              ← Phase accumulator, LUT access, ISR callback
│   └── DDS_lut.c                  ← Sine + Saddle LUTs (PROGMEM)
│
HAL/
├── LCD/
│   ├── HLCD_interface.h           ← LCD 4-bit mode API
│   └── HLCD_program.c            ← LCD 4-bit driver
├── PB/
│   ├── HPB_interface.h            ← Push-button with debounce
│   └── HPB_program.c
│
MCAL/
├── DIO/
│   ├── MDIO_interface.h           ← GPIO API
│   └── MDIO_program.c
├── TIMER/
│   ├── MTIMER_interface.h         ← Timer API (Timer0 CTC for DDS, Timer1 for HW square, Timer2 for systick)
│   └── MTIMER_program.c
├── EXTI/
│   ├── MEXTI_interface.h          ← External interrupts
│   └── MEXTI_program.c
│
CONFIG/
├── DDS/
│   └── DDS_config.h               ← Fs, LUT size, frequency limits
├── LCD/
│   └── HLCD_config.h              ← Pin mappings (4-bit PORTC)
├── TIMER/
│   └── MTIMER_config.h            ← Timer0/1/2 config for 16 MHz
├── PB/
│   └── HPB_config.h
│
HW/
├── DDS/
│   └── DDS_private.h              ← Internal DDS state
├── TIMER/
│   └── MTIMER_private.h
│
LIB/
├── STD_TYPES.h
├── BIT_MATH.h
├── DELAY.h
└── REGISTERS.h
```

### 12.1  ISR Design (Critical Path)

```c
/* This is the ENTIRE waveform ISR — called at 62,500 Hz */
ISR(TIMER0_COMP_vect)
{
    g_u32PhaseAcc += g_u32PhaseInc;
    
    u8 index = (u8)(g_u32PhaseAcc >> 24);
    u8 sample;
    
    switch (g_u8WaveType) {
        case DDS_WAVE_SINE:
            sample = pgm_read_byte(&DDS_au8SineLUT[index]);
            break;
        case DDS_WAVE_TRIANGLE:
            sample = (index < 128) ? (index << 1) : ((255 - index) << 1);
            break;
        case DDS_WAVE_SQUARE:
            sample = (g_u32PhaseAcc & 0x80000000UL) ? 255 : 0;
            break;
        case DDS_WAVE_SAWTOOTH:
            sample = index;
            break;
        case DDS_WAVE_SADDLE:
            sample = pgm_read_byte(&DDS_au8SaddleLUT[index]);
            break;
        default:
            sample = 128;
            break;
    }
    
    PORTA = sample;  /* Direct port write — fastest possible */
}
```

**Rules enforced:**

- ❌ No LCD operations
- ❌ No `_delay_ms()`
- ❌ No ADC conversion
- ❌ No floating-point
- ❌ No loops or string operations
- ✅ Only: accumulate, index, lookup, output

### 12.2  Main Loop (Non-Critical Path)

```c
int main(void) {
    /* Initialize all peripherals */
    DIO_voidInit();
    HLCD_voidInit();        /* 4-bit mode */
    DDS_voidInit();         /* Sets up Timer0 CTC, phase accumulator */
    HPB_enumInit(...);      /* Buttons on PD2,PD3,PD4 */
    
    /* Timer1 for HW square */
    MTIMER_voidInit();
    
    /* Enable global interrupts */
    sei();
    
    /* Initial display */
    UI_voidUpdateDisplay();
    
    while (1) {
        /* Every ~20 ms: poll buttons */
        HPB_voidUpdate(&btn_wave);
        HPB_voidUpdate(&btn_up);
        HPB_voidUpdate(&btn_down);
        
        /* Check for waveform change */
        if (HPB_enumGetEdge(&btn_wave, &edge) == OK && edge == HPB_EDGE_PRESSED) {
            DDS_voidCycleWaveform();
            UI_voidUpdateDisplay();
        }
        
        /* Check for frequency change */
        if (HPB_enumGetEdge(&btn_up, &edge) == OK && edge == HPB_EDGE_PRESSED) {
            DDS_voidIncrementFrequency();
            UI_voidUpdateDisplay();
        }
        if (HPB_enumGetEdge(&btn_down, &edge) == OK && edge == HPB_EDGE_PRESSED) {
            DDS_voidDecrementFrequency();
            UI_voidUpdateDisplay();
        }
        
        /* Pace the main loop — use Timer2 systick */
        MTIMER_voidDelayMs(20);
    }
}
```

---

## 13  Safety & Output Protection

### 13.1  Output Protection Circuit

```
LM358 Output ──► 100Ω series resistor ──► Output Connector
                                              │
                                         Schottky diodes
                                         (clamp to ±5V)
```

The 100Ω series resistor:

- Limits short-circuit current to 50 mA max
- Protects the op-amp from capacitive/inductive loads
- Provides a defined output impedance

### 13.2  Power Supply Decoupling

| Location | Capacitor |
| :--- | :--- |
| ATmega32A VCC (pin 10) | 100 nF ceramic + 10 µF electrolytic |
| ATmega32A AVCC (pin 30) | 100 nF ceramic |
| DAC0808 VCC | 100 nF ceramic |
| LM358 VCC | 100 nF ceramic |
| DAC0808 COMP (pin 16) | 100 nF to GND |

### 13.3  Maximum Output Specification

| Parameter | Value |
| :--- | :--- |
| Max Vpp | ~4.5V (limited by LM358 single-supply swing) |
| DC offset | ~2.25V (centered) |
| Max output current | 20 mA (LM358 limit) |
| Output impedance | 100Ω |

---

## 14  Proteus 8.17 SP5 — Complete Component List

### 14.1  Components

| Component | Proteus Name | Qty | Value/Part |
| :--- | :--- | :--- | :--- |
| ATmega32A | ATMEGA32A | 1 | 16 MHz external crystal |
| Crystal | CRYSTAL | 1 | 16 MHz |
| Crystal caps | CAP | 2 | 22 pF |
| DAC0808 | DAC0808 | 1 | — |
| LM358 | LM358 | 1 | Dual op-amp |
| LCD 16×2 | LM016L | 1 | — |
| Push buttons | BUTTON | 3 | Active-low |
| Potentiometer | POT-LIN | 1 | 10kΩ (amplitude) |
| Resistors | RES | — | See table below |
| Capacitors | CAP | — | See table below |
| Oscilloscope | OSCILLOSCOPE | 1 | Virtual instrument |
| +5V supply | POWER | 1 | +5V |
| −5V supply | POWER | 1 | −5V |
| GND | POWER | — | 0V |

### 14.2  Resistor & Capacitor Values

| Ref | Value | Purpose |
| :--- | :--- | :--- |
| R_REF | 5 kΩ | DAC0808 reference current (1 mA) |
| R_F | 5 kΩ | I-to-V feedback resistor |
| R_FILT | 1 kΩ | Reconstruction filter |
| C_FILT | 10 nF | Reconstruction filter (fc ≈ 16 kHz) |
| R_PROT | 100 Ω | Output protection series resistor |
| C_XTAL | 22 pF × 2 | Crystal load capacitors |
| C_DEC | 100 nF × 4 | Decoupling (MCU, DAC, op-amp) |
| C_BULK | 10 µF | Bulk supply decoupling |
| C_COMP | 100 nF | DAC0808 compensation pin |
| R_PU | Internal | Button pull-ups (ATmega32A internal) |
| POT | 10 kΩ | Output amplitude control |

### 14.3  ATmega32A Proteus Settings

| Setting | Value |
| :--- | :--- |
| Clock Frequency | 16000000 Hz |
| Fuse: CKSEL | External Crystal, 16 MHz |
| Fuse: JTAG | Disabled (frees PC2-PC5 for LCD) |
| Program File | Load .hex from build |

> [!WARNING]
> **JTAG must be disabled** in Proteus (and in the real MCU fuses). By default, ATmega32A has JTAG enabled on PC2–PC5, which conflicts with our LCD D4–D7 pins. In Proteus, set the JTAGEN fuse to "Unprogrammed". On real hardware: write fuse high byte with JTAGEN = 1 (unprogrammed).

---

## 15  Signal Quality Analysis

### 15.1  Factors Limiting Performance

| Factor | Impact | Mitigation |
| :--- | :--- | :--- |
| **8-bit quantization** | 19.5 mV steps, 48 dB SQNR | Acceptable for demonstration |
| **62.5 kHz sample rate** | Nyquist = 31.25 kHz; aliasing above | Reconstruction filter |
| **ISR jitter** | ±1–2 cycles (~62–125 ns) at 16 MHz | Negligible (< 0.4% of sample period) |
| **DAC settling** | DAC0808: 150 ns typical | Well within 16 µs sample period |
| **LM358 slew rate** | 0.3 V/µs limits clean sine to ~15 kHz | Accept distortion above 15 kHz |
| **LM358 bandwidth** | 1.1 MHz GBW | Adequate for ≤ 20 kHz |
| **Reconstruction filter** | 1st-order, -20 dB/decade | Staircase partially visible |
| **Resistor tolerance** | 5% standard | Use 1% for R_REF and R_F |

### 15.2  Expected Oscilloscope Results

| Waveform | Expected Quality |
| :--- | :--- |
| **Sine (1 kHz)** | Clean, smooth sinusoid. ~62 samples/cycle. |
| **Sine (10 kHz)** | Visible but small staircase steps. ~6 samples/cycle. |
| **Triangle (1 kHz)** | Very clean linear ramps. |
| **Square (DAC, 1 kHz)** | Clean transitions, slight RC rounding from filter. |
| **Square (OC1A, 1 kHz)** | Perfect digital edges, 0–5V. |
| **Sawtooth (1 kHz)** | Clean ramp up, rapid reset. Slight RC rounding on reset edge. |
| **Saddle (1 kHz)** | Clearly visible double-hump modified sine. |

---

## 16  Frequency Control System

### 16.1  Control Flow

```
Button Press (PD3 / PD4)
        │
        ▼
Main Loop detects edge
        │
        ▼
Update g_u32DesiredFreq (integer Hz)
        │
        ▼
DDS_voidSetFrequency(g_u32DesiredFreq)
        │
        ▼
Compute g_u32PhaseInc = (freq * 2^32) / Fs   [64-bit intermediate]
        │
        ▼
Atomic write to g_u32PhaseInc (cli/sei guard)
        │
        ▼
Timer1 OCR1A updated for HW square (if square mode)
        │
        ▼
LCD updated with new frequency
```

### 16.2  Frequency Presets

Default frequencies on boot:

| Preset | Frequency |
| :--- | :--- |
| Boot default | 1,000 Hz |
| Minimum | 1 Hz |
| Maximum | 20,000 Hz |

---

## 17  Implementation Phases (Execution Plan)

### Phase 2 — Hardware (Next)

- [ ] Complete Proteus schematic with exact pin connections
- [ ] Verify DAC0808 model in Proteus
- [ ] Verify JTAG fuse disable
- [ ] Simulate power-on with scope probe

### Phase 3 — Software

- [ ] Refactor LCD to 4-bit mode on PORTC
- [ ] Update `F_CPU` to 16 MHz throughout
- [ ] Reconfigure Timer0 for DDS (CTC, prescaler=1, OCR0=255)
- [ ] Reconfigure Timer2 for systick at 16 MHz (OCR2=249)
- [ ] Implement DDS service (phase accumulator, LUTs, ISR)
- [ ] Implement Timer1 CTC toggle for hardware square wave
- [ ] Implement button handler (waveform cycling, frequency adjust)
- [ ] Implement LCD UI
- [ ] Integrate main.c state machine

### Phase 4 — Testing

- [ ] Verify each waveform on Proteus oscilloscope
- [ ] Measure frequency accuracy
- [ ] Verify frequency stepping
- [ ] Verify LCD display
- [ ] Test edge cases (1 Hz, 20 kHz, button rapid press)

---

## Open Questions

> [!IMPORTANT]
> **Q1: Amplitude Control** — Which approach do you prefer?
>
> 1. **(Recommended)** Passive potentiometer in the analog output path — no software/ADC changes needed
> 2. PORTA multiplexing between DAC and ADC (complex, glitchy)
> 3. Digital potentiometer via SPI (extra IC)

> [!IMPORTANT]
> **Q2: Power Supply** — The DAC0808 works best with ±5V (VEE needs negative rail). Options:
>
> 1. **(Recommended)** Use ±5V in Proteus (two voltage sources) and plan for ICL7660 charge pump on real hardware
> 2. Single +5V only — requires alternative DAC circuit or accepting reduced output swing

> [!IMPORTANT]
> **Q3: Maximum Frequency** — Are you comfortable with:
>
> - 10 kHz for clean analog waveforms (>6 samples/cycle)
> - 20 kHz for acceptable analog waveforms (3+ samples/cycle, some aliasing visible)
> - 100+ kHz for hardware square wave on OC1A (digital only)

> [!NOTE]
> **Q4: Crystal Frequency** — Your current codebase uses 8 MHz. The function generator benefits significantly from 16 MHz (doubles sample rate headroom). Should we proceed with 16 MHz, or stay at 8 MHz?
