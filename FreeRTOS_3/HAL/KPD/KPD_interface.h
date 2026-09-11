#ifndef KPD_INTERFACE_H_
#define KPD_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#define KPD_NOT_PRESSED 0xFF

void KPD_voidInit(void);
u8 KPD_u8ScanKey(void);
u8 KPD_u8GetPressedKey(void);

#endif /* KPD_INTERFACE_H_ */
