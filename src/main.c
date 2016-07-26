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

volatile unsigned int *gpio = (unsigned int *) GPIOAO_BASE;

void main(void)
{
	volatile unsigned int *ao_reg =  (unsigned int *) AO_REG;
	volatile unsigned int *ao_reg2 = (unsigned int *) AO_REG2;
	
	/* Reset mux */
	CLR_MASK(*ao_reg, AO_REG_GPIOAO_13_MASK);
	CLR_MASK(*ao_reg2, AO_REG2_GPIOAO_13_MASK);
	
	/* Set as an output */
	CLR_MASK(gpio[GPIOAO_OEN_OFFSET], BIT2MASK(GPIOAO_13_OEN_BIT));
	CLR_MASK(gpio[GPIOAO_PUPDEN_OFFSET], BIT2MASK(GPIOAO_13_PUPDEN_BIT));
	
	/* Set high */
	SET_MASK(gpio[GPIOAO_OUT_OFFSET], BIT2MASK(GPIOAO_13_OUT_BIT));	
	/* Set low */
	CLR_MASK(gpio[GPIOAO_OUT_OFFSET], BIT2MASK(GPIOAO_13_OUT_BIT));	
	
	for(;;)
		;
}
