/************************************************************************\
 * ODROID C2 Bare Metal                                                  *
 * Copyright (C) 2016  Federico "MrModd" Cosentino                       *
 *                                                                       *
 * This program is free software: you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 \***********************************************************************/

#ifndef ODROIDC2_UART_H
#define ODROIDC2_UART_H

#ifndef ODROIDC2_H
#error Please, do not include directly any board specific header file except odroidc2.h
#endif

#include "odroidc2.h"

#define BAUD_RATE 115200
#define UART_CLK 24000000

/* UART_AO_A */

#define AO_REG_GPIOAO_0_FUNC1 12
#define AO_REG_GPIOAO_0_FUNC2 26
#define AO_REG_GPIOAO_1_FUNC1 11
#define AO_REG_GPIOAO_1_FUNC2 25

#define AO_REG_UART_AO_A_MASK                    \
		((1u << AO_REG_GPIOAO_0_FUNC1) | \
		(1u << AO_REG_GPIOAO_1_FUNC1))

#define UART_AO_A_BASE 0xc81004c0

iomemdef(UART0_AO_WFIFO, (UART_AO_A_BASE + 0));
iomemdef(UART0_AO_RFIFO, (UART_AO_A_BASE + 4));
iomemdef(UART0_AO_CONTROL, (UART_AO_A_BASE + 8));
iomemdef(UART0_AO_STATUS, (UART_AO_A_BASE + 12));
iomemdef(UART0_AO_MISC, (UART_AO_A_BASE + 16));
iomemdef(UART0_AO_REG5, (UART_AO_A_BASE + 20));

/* Control bits */

#define UART0_AO_CONTROL_TX_EN (1u << 12)
#define UART0_AO_CONTROL_RX_EN (1u << 13)
#define UART0_AO_CONTROL_TWO_WIRE (1u << 15)
#define UART0_AO_CONTROL_CHAR_LEN_MASK (3u << 20)
#define UART0_AO_CONTROL_RST_TX (1u << 22)
#define UART0_AO_CONTROL_RST_RX (1u << 23)
#define UART0_AO_CONTROL_CLR_ERROR (1u << 24)
#define UART0_AO_CONTROL_RX_INT_EN (1u << 27)
#define UART0_AO_CONTROL_TX_INT_EN (1u << 28)

/* Status bits */

#define UART0_AO_STATUS_ERRORS (7u << 16) /* Includes Parity, Frame and TX FIFO overflow errors */
#define UART0_AO_STATUS_RX_FULL (1u << 19)
#define UART0_AO_STATUS_RX_EMPTY (1u << 20)
#define UART0_AO_STATUS_TX_FULL (1u << 21)
#define UART0_AO_STATUS_TX_EMPTY (1u << 22)
#define UART0_AO_STATUS_XMIT_BUSY (1u << 25)
#define UART0_AO_STATUS_RECV_BUSY (1u << 26)

/* REG5 bits */

#define UART0_AO_REG5_BAUD_VAL_MASK (0x7FFFFF << 0)
#define UART0_AO_REG5_USE_BAUD (1u << 23)
#define UART0_AO_REG5_XTAL_CLK (1u << 24)

#endif
