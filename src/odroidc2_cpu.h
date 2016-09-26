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

#include "types.h"

/* Definition of inline assembly:
 * 
 * asm ("instruction1" "instruction2" "instruction3");
 * or
 * __asm__ ("instruction1" "instruction2" "instruction3");
 * 
 * __asm__ is preferable because is ISO C
 * 
 * 
 * 
 * Definition of extended assembly:
 * it uses operands too:
 * 
 * __asm__ ("instruction1"
 * 				: output operands
 * 				: input operands
 * 				: list of cobbler registers
 * 		);
 * 
 * 
 * 
 * For further details see http://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
 * 
 */

#define CURRENT_EL0 0x00
#define CURRENT_EL1 0x04
#define CURRENT_EL2 0x08
#define CURRENT_EL3 0x0c

/**
 * _read_CurrentEL: get the current exception level
 * returns: a 64bit value where bits [3:2] encodes the
 *          current exception level as follow:
 *          - 00 -> EL0
 *          - 01 -> EL1
 *          - 10 -> EL2
 *          - 11 -> EL3
 */
#define _read_CurrentEL() __extension__ ({	\
	u64 value;          	\
	__asm__ __volatile__ ("mrs %[reg], CurrentEL" : [reg] "=r" (value) : : "memory"); \
	value; })
/* Macro above works only with GCC and it is not ANSI/ISO C compliant. */

/* Next instructions are three different kinds of barriers.
 * In ARMv7 these were obtained with the co-processor C15.
 * In ARMv8 there are specific instructions that take as
 * argument the kind of synchronization you want to obtain.
 * On the Programmer's Guide look for the table "Barriers
 * Parameters".
 * "SY" stands for synchronize both reads and writes at a
 * full system domain.
 */

/**
 * __isb: Instruction Synchronization Barrier
 *
 * Guarantees that subsequent instructions are fetched
 * after the barrier, eventually with a different
 * privilege level.
 * This is usefull for example when the behaviour of
 * an instruction depends on what exception level is
 * currently running.
 */
#define __isb() __asm__ __volatile__ ("ISB SY" : : : "memory")

/**
 * __dmb: Data Memory Barrier
 *
 * Prevents reordering of data accesses instructions
 * accross the barrier instruction.
 * A load operation, for example, is delayed until all
 * previous memory accesses are synchronized.
 */
#define __dmb() __asm__ __volatile__ ("DMB SY" : : : "memory")

/**
 * __dsb: Data Synchronization Barrier
 *
 * Like DMB, but blocking the execution until
 * synchronization is complete.
 * Not just load and store operations are delayed.
 */
#define __dsb() __asm__ __volatile__ ("DSB SY" : : : "memory")

#endif
