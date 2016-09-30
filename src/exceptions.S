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

/*
 * Exceptions in ARMv8
 *
 * ARMv8 simplifies exception handling of ARMv7
 * by grouping them into 4 types:
 * - synchronous exceptions: generated attepting the execution of an
 *   instruction (eg. software interrupt or data/instruction aborts);
 * - IRQ: the normal priority interrupts;
 * - FIQ: the high priority interrupts;
 * - SError: all the asynchronous exceptions but IRQ and FIQ (eg.
 *   asynchronous data abort caused by writeback).
 *
 * When an exception occurs the processor can be in one of the 4
 * exception level EL0-EL3 and can be in AArch32 or AArch64 execution
 * state. The ARMv8 exception vector table contains different entries
 * for each of them.
 *
 * Most important ARMv8 introduces more space per entry in the
 * exception vector table: 16 instructions instead of the single
 * instruction of ARMv7 architecture. This allow to handle the
 * exception directly from the exception vector instead of the
 * usual jump instruction (LDR_PC_PC).
 *
 * See the "AArch64 exception table" paragraph in the ARMv8
 * Programmer's Guide.
 */

	/* Force alignment of the code */
	.align 11
	.global _exception_vector

_exception_vector:

/*
 * ============================
 * Exceptions catched from
 * the same EL with a single SP
 * ============================
 */

//.align 7
_sync_current_EL_SP0:
	bl panic0

/* During compilation .org directive
 * forces the PC to move forward by
 * the amount specified (e.g. 0x080).
 * In this way you can space the
 * instructions of the exception vector
 * table because each entry must be
 * exactly 16 instruction length. */

.org 0x080
_irq_current_EL_SP0:
	bl panic0

.org 0x100
_fiq_current_EL_SP0:
	bl panic0

.org 0x180
_serror_current_EL_SP0:
	bl panic0

/*
 * ============================
 * Exceptions catched from
 * the same EL with a SP for
 * each EL
 * ============================
 */

.org 0x200
_sync_current_EL_SPx:
	bl panic0

.org 0x280
_irq_current_EL_SPx:
	bl panic0

.org 0x300
_fiq_current_EL_SPx:
	bl panic0

.org 0x380
_serror_current_EL_SPx:
	bl panic0

/*
 * ============================
 * Exceptions catched from
 * the lower EL during AArch64
 * execution state
 * ============================
 */

.org 0x400
_sync_lower_EL_AArch64:
	bl panic0

.org 0x480
_irq_lower_EL_AArch64:
	bl panic0

.org 0x500
_fiq_lower_EL_AArch64:
	bl panic0

.org 0x580
_serror_lower_EL_AArch64:
	bl panic0

/*
 * ============================
 * Exceptions catched from
 * the lower EL during AArch32
 * execution state
 * ============================
 */

.org 0x600
_sync_lower_EL_AArch32:
	bl panic0

.org 0x680
_irq_lower_EL_AArch32:
	bl panic0

.org 0x700
_fiq_lower_EL_AArch32:
	bl panic0

.org 0x780
_serror_lower_EL_AArch32:
	bl panic0

.org 0x800
