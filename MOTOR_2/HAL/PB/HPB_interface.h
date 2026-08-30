/*
 * HPB_interface.h
 *
 *  Created on: Aug 26, 2026
 *      Author: Anthony Gaius
 */

#ifndef HPB_INTERFACE_H_
#define HPB_INTERFACE_H_

#include "../../LIB/STD_TYPES.h"

/* Button Connection Types */
#define HPB_PULL_DOWN   0
#define HPB_PULL_UP     1

/* Button States */
#define HPB_PRESSED     1
#define HPB_RELEASED    0

/**
 * @brief Initialize a push button pin.
 * @param Copy_u8Port: The port the button is connected to.
 * @param Copy_u8Pin: The pin the button is connected to.
 * @param Copy_u8PullType: HPB_PULL_DOWN or HPB_PULL_UP (activates internal pull-up).
 */
void HPB_voidInit(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType);

/**
 * @brief Get the logical state of the push button.
 * @param Copy_u8Port: The port the button is connected to.
 * @param Copy_u8Pin: The pin the button is connected to.
 * @param Copy_u8PullType: HPB_PULL_DOWN or HPB_PULL_UP.
 * @param Copy_pu8State: Pointer to store the result (HPB_PRESSED or HPB_RELEASED).
 */
void HPB_voidGetState(u8 Copy_u8Port, u8 Copy_u8Pin, u8 Copy_u8PullType, u8* Copy_pu8State);

#endif /* HPB_INTERFACE_H_ */
