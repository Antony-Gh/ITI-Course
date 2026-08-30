#ifndef HLED_INTERFACE_H_
#define HLED_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

void HLED_voidInit(void);
void HLED_voidWritePattern(u8 Copy_u8Pattern);
void HLED_voidSetLed(u8 Copy_u8LedIdx, u8 Copy_u8State);
void HLED_voidClearAll(void);

#endif /* HLED_INTERFACE_H_ */
