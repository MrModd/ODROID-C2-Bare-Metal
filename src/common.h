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

#ifndef COMMON_H
#define COMMON_H

#include "types.h"

extern void main(void);

/**
 * loop_delay: wait a bit doing some empty loops
 * @d: number of loops to do
 */
static inline void loop_delay(u64 d)
{
	/* Making c volatile prevents compiler to
	 * optimize next useless while loop */
	volatile u64 c = 0;

	while (d-- > 0) {
		c+=d;
	}
}

#endif
