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

/**
 * init_exceptions: set the Vector Base Address
 *                  Register if in EL1-EL3
 */
static void init_exceptions(void)
{
	extern void _exception_vector(void);
	u64 current_el = _read_CurrentEL();

	if (current_el != CURRENT_EL0) {
		__asm__ __volatile__ ("msr VBAR_EL1, %[vect]" : : [vect] "r" (&_exception_vector) : "memory");
	}
}

/**
 * init_bss: initialze bss section setting to
 *           zero all the content
 */
static void init_bss(void)
{
	extern u64 _bss_start, _bss_end;
	u64 *p;

	for (p = &_bss_start; p < &_bss_end; p++) {
		*p = 0llu;
	}
}

/**
 * init_gpio: set GPIO port wired to the system led
 *            as an output and clear the pinmux
 */
static void init_gpio(void)
{
	/* Reset mux */
	iomem_low(AO_REG, AO_REG_GPIOAO_13_MASK);
	iomem_low(AO_REG2, AO_REG2_GPIOAO_13_MASK);
	
	/* Set as an output */
	iomem_low(GPIOAO_OEN, BIT2MASK(GPIOAO_13_OEN_BIT));
	iomem_low(GPIOAO_PUPDEN, BIT2MASK(GPIOAO_13_PUPDEN_BIT));
}

void _init(void)
{
	init_exceptions();
	init_bss();
	init_gpio();

	main();
}
