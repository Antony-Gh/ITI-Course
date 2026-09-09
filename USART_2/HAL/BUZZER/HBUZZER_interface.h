/*
 * HBUZZER_interface.h
 *
 *  Created on: Sep 1, 2026
 *      Author: Anthony Gaius
 */

#ifndef HBUZZER_INTERFACE_H_
#define HBUZZER_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

void HBUZZER_voidInit(void);
void HBUZZER_voidSetSection(u8 Copy_u8Section);
void HBUZZER_voidStop(void);

/* Set tone directly by OCR1A compare value (for UART-controlled second MCU) */
void HBUZZER_voidSetToneByOCR(u16 Copy_u16OcrValue);

#endif /* HBUZZER_INTERFACE_H_ */
