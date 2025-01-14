/** @file hal/micro/cortexm3/efm32/board/brd4156a.h
 * BRD4156A-EFR32MG12 2.4 GHz / 915 MHz 19.5dBm
 * See @ref board for detailed documentation.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

/** @addtogroup board
 *  @brief Functions and definitions specific to the breakout board.
 *
 *@{
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "bspconfig.h"

// Define board features

// #define DISABLE_PTI

// #define DISABLE_DCDC

// Enable COM port to use retarget serial configuration
#define COM_RETARGET_SERIAL
// Enable HW flow control
#define COM_USART0_HW_FC

// Define nHOST_INT to PF6 -> EXP7
#define NHOST_INT_PORT gpioPortF
#define NHOST_INT_PIN  6
// Define nWAKE to PF7 -> EXP9
#define NWAKE_PORT     gpioPortF
#define NWAKE_PIN      7

// Include common definitions
#include "board/wstk-common.h"

#endif //__BOARD_H__
/** @} END Board Specific Functions */

/** @} END addtogroup */
