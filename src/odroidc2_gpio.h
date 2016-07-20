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

#ifndef ODROIDC2_GPIO
#define ODROIDC2_GPIO
 
#define GPIO_BASE   0xc8834400
#define GPIOAO_BASE 0xc8100000

#define AO_REG  0xc8100014
#define AO_REG2 0xc8100018

#define GPIOAO_OFF1 0x09
#define GPIOAO_OFF2 0x0a
#define GPIOAO_OFF3 0x0b

#define LED_BIT 13

#endif
