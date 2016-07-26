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

#ifndef ODROIDC2_GPIO_H
#define ODROIDC2_GPIO_H

#ifndef ODROIDC2_H
#error Please, do not include any board specific header file except odroidc2.h
#endif

/* Refer to the SoC manual for all the addresses referred to some devices
 * such as the GPIO */



/* === GPIO bank AO === */

/* PINMUX registers */
#define AO_REG  0xc8100014
#define AO_REG2 0xc8100018

/* For each GPIO different functions can be selected.
 * Each function is selectable asserting the correct
 * bit on the PINMUX registers.
 * If all functions are disabled the GPIO will act
 * as a simple digital I/O pin.
 * GPIOAO_13 (the one linked to the LED) has 4
 * functions selectable from the registers
 * AO_REG and AO_REG2 (See SoC manual).
 * The following definition specify the bit related
 * to the given function. */
#define AO_REG_GPIOAO_13_FUNC1  31
#define AO_REG_GPIOAO_13_FUNC2  4
#define AO_REG_GPIOAO_13_FUNC3  3
#define AO_REG2_GPIOAO_13_FUNC4 1

/* From the bits definition a register mask is created */
#define AO_REG_GPIOAO_13_MASK                    \
	(                                        \
		(1u << AO_REG_GPIOAO_13_FUNC1) | \
		(1u << AO_REG_GPIOAO_13_FUNC2) | \
		(1u << AO_REG_GPIOAO_13_FUNC3)   \
	)
#define AO_REG2_GPIOAO_13_MASK                   \
	(                                        \
		(1u << AO_REG2_GPIOAO_13_FUNC4)  \
	)



/* Control registers
 * Manage the direction, the value and the Pull-Up/Down
 * resistor for each GPIO.
 * Each bank has its own base address and the registers
 * are located at a given offset from it. */
#define GPIOAO_BASE 0xc8100000 /* Base address */

/* Registers offsets from the GPIOAO_BASE address:
 *
 *   final_address = GPIOAO_BASE + <OFFSET> * 4
 */
#define GPIOAO_OEN_OFFSET     0x09
#define GPIOAO_OUT_OFFSET     0x09
#define GPIOAO_IN_OFFSET      0x0a
#define GPIOAO_PUPDEN_OFFSET  0x0b
#define GPIOAO_PUPD_OFFSET    0x0b

#define GPIOAO_13             13

#define GPIOAO_13_OEN_BIT     (0  + GPIOAO_13)
#define GPIOAO_13_OUT_BIT     (16 + GPIOAO_13)
#define GPIOAO_13_IN_BIT      (0  + GPIOAO_13)
#define GPIOAO_13_PUPDEN_BIT  (0  + GPIOAO_13)
#define GPIOAO_13_PUPD_BIT    (16 + GPIOAO_13)
#endif
