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

/* State of the CPU:
 *   ELxt --> State x using SP_EL0 as stack pointer
 *   ELxh --> State x using SP_ELx as stack pointer
 */
#define MODE_EL0t		0x00000000
#define MODE_EL1t		0x00000004
#define MODE_EL1h		0x00000005
#define MODE_EL2t		0x00000008
#define MODE_EL2h		0x00000009
#define MODE_EL3t		0x0000000c
#define MODE_EL3h		0x0000000d

/* Bit values for SPSR register in Aarch64 execution mode */
#define SPSR_F		0x00000040
#define SPSR_I		0x00000080
#define SPSR_A		0x00000100
#define SPSR_D		0x00000200

/* Give some registers a name */
stack			.req x0
curr_el			.req x1
tmp			.req x2

	.text
	.global _reset

_reset:
	/* Set the stack pointer initial address
	 * to the end of memory */

	/* Copy the address of stack_top (defined in
	 * program.lds script) to a 64bit reg.
	 * This will be copied to the right SP reg. */
	ldr stack, =stack_top

	/* Check current Exception Level */
	/* From the Architecture Reference Manual:
	* CurrentEL contains indications about the current EL state on bits
	 * [3:2].
	 *   - 00 -> EL0
	 *   - 01 -> EL1
	 *   - 10 -> EL2
	 *   - 11 -> EL3
	 * The reset state depends on the implementation.
	 */

	mrs curr_el, CurrentEL	/* Copy special reg CurrentEL to 64bit reg x0 */

	cmp curr_el, #MODE_EL2t
	ccmp curr_el, #MODE_EL2h, #0x4, ne	/* Conditional compare:
                                          	 * If current condition code in NZCV flags
                                          	 * is "ne" compare x0 and #MODE_EL3h, else
                                          	 * set #0x4 as NZCV flag mask (see the
                                          	 * Programmer's guide for all possible
                                          	 * condition codes); #0x4 correspond to
                                          	 * a negative condition. */
	b.ne 1f				/* Branch to state2 if CurrentEL is not in EL2t nor EL2h */

2:
	/* Set little endianess */
	mrs tmp, SCTLR_EL2
	bic tmp, tmp, #(1 << 25)	/* Clear EE bit */
	msr SCTLR_EL2, tmp
	/* Set EL1 to 64bit */
	mrs tmp, HCR_EL2
	orr tmp, tmp, #(1<<31)		/* Set RW bit */
	msr HCR_EL2, tmp
	/* Set Stack Pointer */
	msr SP_EL1, stack

	bl .Ldrop_privileges
	/* Now we are in state EL1 */

1:
	/* Set little endianess */
	mrs tmp, SCTLR_EL1
	bic tmp, tmp, #((1 << 25) | (1 << 24))	/* Clear EE and EOE bits */
	msr SCTLR_EL1, tmp

	/* End of processor initialization */
	b _init

.Ldrop_privileges:
	mov tmp, #0x0800
	movk tmp, #0x30d0, lsl #16	/* Init System control reg for EL1 */
	/* In order to load a 32bit value into tmp two steps are required.
	 * mov loads lower 16bits while movk loads higher 16bits using a
	 * logical left shift of 16 positions.
	 * The assembled value is 0x30d00800 */
	msr SCTLR_EL1, tmp		/* Store new value to the special register */

	mov tmp, #(SPSR_F|SPSR_I|SPSR_A|SPSR_D|MODE_EL1h)
	msr SPSR_EL2, tmp		/* Drop to EL1 mode with IRQ/FIQ masked */

	adr tmp, _ret
	msr ELR_EL2, tmp		/* Exception Link Register is the register used
                        		 * to jump back to the caller when an exception
                        		 * occurred. Since LR register is not exposed use
                        		 * a trick to use that value: set ELR_EL2 to a
                        		 * label corresponding to a ret instruction. */
	eret				/* Set as PSTATE the one written in SPSR_EL2 and return */

	.global _ret

_ret:
	ret
