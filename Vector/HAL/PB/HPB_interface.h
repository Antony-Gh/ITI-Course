/*
 * HPB_interface.h
 *
 *  Created on: Aug 26, 2026
 *      Author: Anthony Gaius
 */

#ifndef HPB_INTERFACE_H_
#define HPB_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

#include "../../CONFIG/PB/HPB_config.h"

/* Button Connection Types */
#define HPB_PULL_DOWN 0
#define HPB_PULL_UP 1

/* Button States */
#define HPB_PRESSED 1
#define HPB_RELEASED 0

/* Button Edge Events */
#define HPB_EDGE_NONE 0
#define HPB_EDGE_PRESSED 1
#define HPB_EDGE_RELEASED 2

typedef struct {
  u8 Port;
  u8 Pin;
  u8 PullType;
  u8 DebouncedState;
  u8 PreviousDebouncedState;
  u8 LastStableRaw;
  u8 StableCount;
} HPB_t;

STD_err_e HPB_enumInit(HPB_t *Copy_pstButton, u8 Copy_u8Port, u8 Copy_u8Pin,
                       u8 Copy_u8PullType);
void HPB_voidUpdate(HPB_t *Copy_pstButton);
STD_err_e HPB_enumGetState(const HPB_t *Copy_pstButton, u8 *Copy_pu8State);
STD_err_e HPB_enumGetEdge(HPB_t *Copy_pstButton, u8 *Copy_pu8Edge);

/* Legacy API */
void HPB_voidInit(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType);
void HPB_voidGetState(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType,
                      u8 *Copy_pu8State);

#endif /* HPB_INTERFACE_H_ */
