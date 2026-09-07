/*
 * MEXTI_private.h
 *
 *  Created on: Aug 30, 2026
 *      Author: Anthony Gaius
 */

#ifndef MEXTI_PRIVATE_H_
#define MEXTI_PRIVATE_H_

#include "../../CONFIG/EXTI/MEXTI_config.h"

/* GICR bits */
#define EXTI_GICR_INT2_BIT 5U
#define EXTI_GICR_INT0_BIT 6U
#define EXTI_GICR_INT1_BIT 7U

/* GIFR bits */
#define EXTI_GIFR_INTF2_BIT 5U
#define EXTI_GIFR_INTF0_BIT 6U
#define EXTI_GIFR_INTF1_BIT 7U

/* MCUCR bits for INT0 / INT1 */
#define EXTI_MCUCR_ISC00_BIT 0U
#define EXTI_MCUCR_ISC01_BIT 1U
#define EXTI_MCUCR_ISC10_BIT 2U
#define EXTI_MCUCR_ISC11_BIT 3U

/* MCUCSR bit for INT2 */
#define EXTI_MCUCSR_ISC2_BIT 0U

#endif /* MEXTI_PRIVATE_H_ */
