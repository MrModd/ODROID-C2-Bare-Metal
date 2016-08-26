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

#ifndef ODROIDC2_H
#define ODROIDC2_H

#include "types.h"

/* Utility functions */

/* Set the n-th bit of a 32bit variable */
#define BIT2MASK(bit)            \
		(1u<<(bit))

/* Set the n-th bit of a 64bit variable */
#define BIT2MASK_64(bit)         \
		(1ull<<(bit))

/* Set a bitmask in a register preserving
 * other bits */
#define SET_MASK(reg, mask)      \
		reg |= (mask)

/* Clear a bitmask in a register preserving
 * other bits */
#define CLR_MASK(reg, mask)      \
		reg &= ~(mask)

/* iomem definitions */

/* _iomem is the address that will be used as base address of the GPIO
 * address space.
 * It is important to set it to zero because compiler can make
 * optimizations when accessing to an offset of it, because the sum
 * of a number with zero is always the number itself.
 * This prevents to load the constant value from memory every time.
 * See also the iomem(N) definition below. */
static volatile u32 * const _iomem = (u32*) 0;

/**
 * iomemdef: generates new constant @N with value @V.
 * This macro is useful in order to define memory addresses at runtime as
 * constants instead of variables.
 * Constants avoid the necessity to fetch the variable value from the memory.
 *
 * @N the name of the new constant
 * @V the value for the new constant
 */
#define iomemdef(N, V) enum { N = (V) / sizeof(u32) }

/**
 * iomem: it is used to access address @N as if it was
 * an offset of the base address _iomem (see above)
 * avoiding RAM accesses required to fetch the value of
 * @N itself. In fact @N is a constant defined by
 * iomemdef and does not require a memory access to get
 * its value, moreover the (_iomem + N) sum is simplified
 * by the compiler with just N because _iomem is 0.
 *
 * @N the memory address to be accessed
 */
#define iomem(N) _iomem[N]

/**
 * iomem_high: set to 1 all bits of register @reg that
 * are marked as 1 in @mask.
 *
 * @reg: a memory address corresponding to a 32bit register
 * @mask: a bitmask to be set in the register
 */
static inline void iomem_high(u32 reg, u32 mask)
{
	SET_MASK(iomem(reg), mask);
}

/**
 * iomem_low: set to 0 all bits of register @reg that
 * are marked as 1 in @mask.
 *
 * @reg: a memory address corresponding to a 32bit register
 * @mask: a bitmask to be cleared in the register
 */
static inline void iomem_low(u32 reg, u32 mask)
{
	CLR_MASK(iomem(reg), mask);
}

/* Other architecture specific header files */

#include "odroidc2_gpio.h"
#include "odroidc2_cpu.h"

#endif
