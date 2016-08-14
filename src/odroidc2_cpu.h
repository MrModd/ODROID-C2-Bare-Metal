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

#ifndef ODROIDC2_CPU_H
#define ODROIDC2_CPU_H

#ifndef ODROIDC2_H
#error Please, do not include directly any board specific header file except odroidc2.h
#endif

#include "common.h"

#define _read_CurrentEL() ({	\
	u64 value;          	\
	__asm__ __volatile__ ("mrs %[reg], CurrentEL" : [reg] "=r" (value) : : "memory"); \
	value; })

#endif
