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

#include "odroidc2.h"
#include "common.h"

/* From SoC manual you can see the length of these registers. Although
 * ARM 64bit allow to have 64bit addresses, these ones are 32 bit long.
 * Be careful on the cast you make. */

void main(void)
{
	/* Blinking the SYS_LED indefinitely */

	for(;;) {

		/* Set high */
		iomem_high(GPIOAO_OUT, BIT2MASK(GPIOAO_13_OUT_BIT));

		loop_delay(1000000llu);
	
		/* Set low */
		iomem_low(GPIOAO_OUT, BIT2MASK(GPIOAO_13_OUT_BIT));

		loop_delay(1000000llu);
	}
}
