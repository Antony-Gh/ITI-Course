/*
 * DDS_program.c
 *
 *  Created on: Sep 14, 2026
 *      Author: Anthony Gaius
 *
 *  DDS (Direct Digital Synthesis) waveform engine.
 *
 *  Architecture:
 *    Timer0 CTC ISR fires at Fs = 62,500 Hz.
 *    Each ISR tick:
 *      1. Advance 32-bit phase accumulator by phaseIncrement
 *      2. Extract upper 8 bits as LUT index
 *      3. Look up / compute waveform sample
 *      4. Write sample to PORTA (DAC0808)
 *
 *  Timer1 CTC toggle mode generates a jitter-free hardware square wave
 *  on OC1A (PD5), independent of the DAC path.
 *
 *  Phase increment calculation:
 *    phaseInc = (f_desired * 2^32) / Fs
 *  Uses 64-bit intermediate to avoid overflow.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../../LIB/BIT_MATH.h"
#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/DDS/DDS_config.h"
#include "../../HW/DDS/DDS_private.h"
#include "DDS_interface.h"

/* ====================================================================
 *  External LUT declarations (defined in DDS_lut.c)
 * ==================================================================== */
extern const u8 DDS_au8SineLUT[DDS_LUT_SIZE];
extern const u8 DDS_au8SaddleLUT[DDS_LUT_SIZE];

/* ====================================================================
 *  Module-level state (volatile — shared with ISR)
 * ==================================================================== */
static volatile u32 s_u32PhaseAcc = 0;
static volatile u32 s_u32PhaseInc = 0;
static volatile u8 s_u8WaveType = DDS_WAVE_SINE;

/* Current frequency in Hz (non-volatile, only touched in main context) */
static u32 s_u32CurrentFreqHz = DDS_FREQ_DEFAULT;

/* Waveform name strings */
static const char s_acWaveNames[DDS_WAVE_COUNT][9] = {
    "SINE    ", "TRIANGLE", "SQUARE  ", "SAWTOOTH", "SADDLE  "};

/* ====================================================================
 *  Private: Compute phase increment from frequency
 * ==================================================================== */
static u32 DDS_u32CalcPhaseInc(u32 Copy_u32FreqHz) {
  /*
   * phaseInc = (f * 2^32) / Fs
   * 
   * Fs = F_CPU / 256 (Timer1 Fast PWM overflow rate)
   * phaseInc = (f * 2^32 * 256) / F_CPU
   */
  u64 local_u64Temp = (u64)Copy_u32FreqHz * DDS_PHASE_ACC_FULL_SCALE * 256ULL;
  return (u32)(local_u64Temp / F_CPU);
}

/* ====================================================================
 *  Private: Get adaptive frequency step size
 * ==================================================================== */
static u32 DDS_u32GetStep(u32 Copy_u32FreqHz) {
  if (Copy_u32FreqHz >= 10000UL) {
    return DDS_STEP_1KHZ;
  } else if (Copy_u32FreqHz >= 1000UL) {
    return DDS_STEP_100HZ;
  } else if (Copy_u32FreqHz >= 100UL) {
    return DDS_STEP_10HZ;
  } else {
    return DDS_STEP_1HZ;
  }
}

/* ====================================================================
 *  Private: Update Timer1 OCR1A for hardware square wave
 *
 *  f_square = F_CPU / (2 * 1 * (OCR1A + 1))
 *  OCR1A = (F_CPU / (2 * f_desired)) - 1
 * ==================================================================== */
/* Removed DDS_voidUpdateHwSquare since Timer1 is now the DAC */

/* ====================================================================
 *  Initialization
 * ==================================================================== */
void DDS_voidInit(void) {
  /* ---- Configure PD5 (OC1A) as output for PWM DAC ---- */
  SET_BIT(DDRD, 5U);

  /* ---- Configure PD4 as output for raw Square/Sync wave ---- */
  SET_BIT(DDS_SYNC_DDR, DDS_SYNC_PIN);
  CLR_BIT(DDS_SYNC_PORT, DDS_SYNC_PIN);

  /* ---- Timer1: 8-bit Fast PWM mode on OC1A (PD5) ---- */
  TCCR1A = DDS_TCCR1A_CONFIG;
  TCCR1B = DDS_TCCR1B_CONFIG;
  TCNT1 = 0U;
  OCR1A = 128U; /* Mid-scale (0V DC offset) */

  /* Enable Timer1 Overflow interrupt (Sample Clock directly tied to PWM cycle) */
  SET_BIT(TIMSK, TOIE1);

  /* ---- Set default waveform and frequency ---- */
  s_u8WaveType = DDS_WAVE_SINE;
  s_u32CurrentFreqHz = DDS_FREQ_DEFAULT;
  s_u32PhaseInc = DDS_u32CalcPhaseInc(DDS_FREQ_DEFAULT);
  s_u32PhaseAcc = 0;
}

/* ====================================================================
 *  Waveform Control
 * ==================================================================== */
void DDS_voidSetWaveform(u8 Copy_u8WaveType) {
  if (Copy_u8WaveType < DDS_WAVE_COUNT) {
    s_u8WaveType = Copy_u8WaveType;
    
    /* Ensure sync pin is cleared if not generating a square wave */
    if (Copy_u8WaveType != DDS_WAVE_SQUARE) {
      CLR_BIT(DDS_SYNC_PORT, DDS_SYNC_PIN);
    }
  }
}

u8 DDS_u8GetWaveform(void) { return s_u8WaveType; }

void DDS_voidCycleWaveform(void) {
  u8 local_u8Next = s_u8WaveType + 1U;
  if (local_u8Next >= DDS_WAVE_COUNT) {
    local_u8Next = 0U;
  }
  DDS_voidSetWaveform(local_u8Next);
}

const char *DDS_pcGetWaveformName(void) { return s_acWaveNames[s_u8WaveType]; }

/* ====================================================================
 *  Frequency Control
 * ==================================================================== */
void DDS_voidSetFrequency(u32 Copy_u32FreqHz) {
  /* Clamp to valid range */
  if (Copy_u32FreqHz < DDS_FREQ_MIN) {
    Copy_u32FreqHz = DDS_FREQ_MIN;
  }
  if (Copy_u32FreqHz > DDS_FREQ_MAX) {
    Copy_u32FreqHz = DDS_FREQ_MAX;
  }

  s_u32CurrentFreqHz = Copy_u32FreqHz;

  /* Compute new phase increment (64-bit intermediate) */
  u32 local_u32NewInc = DDS_u32CalcPhaseInc(Copy_u32FreqHz);

  /* Atomic write — the ISR reads s_u32PhaseInc as a 32-bit value,
   * and on an 8-bit AVR this is NOT atomic (4 byte reads).
   * We must disable interrupts briefly. */
  u8 local_u8Sreg = SREG;
  cli();
  s_u32PhaseInc = local_u32NewInc;
  SREG = local_u8Sreg;
}

u32 DDS_u32GetFrequency(void) { return s_u32CurrentFreqHz; }

void DDS_voidIncrementFrequency(void) {
  u32 local_u32Step = DDS_u32GetStep(s_u32CurrentFreqHz);
  u32 local_u32NewFreq = s_u32CurrentFreqHz + local_u32Step;

  if (local_u32NewFreq > DDS_FREQ_MAX) {
    local_u32NewFreq = DDS_FREQ_MAX;
  }

  DDS_voidSetFrequency(local_u32NewFreq);
}

void DDS_voidDecrementFrequency(void) {
  u32 local_u32Step = DDS_u32GetStep(s_u32CurrentFreqHz);

  if (s_u32CurrentFreqHz <= local_u32Step) {
    DDS_voidSetFrequency(DDS_FREQ_MIN);
  } else {
    DDS_voidSetFrequency(s_u32CurrentFreqHz - local_u32Step);
  }
}

/* ====================================================================
 *  Timer1 Overflow ISR — Waveform Sample Generation
 *
 *  This ISR executes exactly once per PWM cycle (31,250 Hz),
 *  ensuring perfect synchronization between the DDS and the DAC.
 *
 *  Rules:
 *    - NO LCD operations
 *    - NO delay functions
 *    - NO ADC reads
 *    - NO floating-point
 *    - NO loops or string ops
 *    - ONLY: accumulate → index → lookup → output
 * ==================================================================== */
ISR(TIMER1_OVF_vect) {
  /* 1. Advance phase accumulator */
  s_u32PhaseAcc += s_u32PhaseInc;

  /* 2. Extract upper 8 bits as LUT index */
  u8 local_u8Index = (u8)(s_u32PhaseAcc >> DDS_LUT_INDEX_SHIFT);

  /* 3. Generate waveform sample based on type */
  u8 local_u8Sample;

  switch (s_u8WaveType) {

  case DDS_WAVE_SINE:
    local_u8Sample = pgm_read_byte(&DDS_au8SineLUT[local_u8Index]);
    break;

  case DDS_WAVE_TRIANGLE:
    /* Rising ramp 0→254 for index 0→127, falling 254→0 for 128→255 */
    if (local_u8Index < 128U) {
      local_u8Sample = (u8)(local_u8Index << 1);
    } else {
      local_u8Sample = (u8)((255U - local_u8Index) << 1);
    }
    break;

  case DDS_WAVE_SQUARE:
    /* MSB of accumulator determines high/low */
    if (s_u32PhaseAcc & DDS_PHASE_MSB_MASK) {
      local_u8Sample = 255U;
      SET_BIT(DDS_SYNC_PORT, DDS_SYNC_PIN);
    } else {
      local_u8Sample = 0U;
      CLR_BIT(DDS_SYNC_PORT, DDS_SYNC_PIN);
    }
    break;

  case DDS_WAVE_SAWTOOTH:
    /* Upper byte of accumulator IS the sawtooth */
    local_u8Sample = local_u8Index;
    break;

  case DDS_WAVE_SADDLE:
    local_u8Sample = pgm_read_byte(&DDS_au8SaddleLUT[local_u8Index]);
    break;

  default:
    local_u8Sample = 128U; /* Mid-scale fallback */
    break;
  }

  /* 4. Output to DAC via Timer1 OCR1A (Fast PWM) */
  OCR1A = local_u8Sample;
}
