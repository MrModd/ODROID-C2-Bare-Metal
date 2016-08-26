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

#include "odroidc2_gpio.h"

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

#endif
