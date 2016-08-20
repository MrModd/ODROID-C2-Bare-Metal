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
#define SPSR_F			0x00000040 /* FIQ mask bit */
#define SPSR_I			0x00000080 /* IRQ mask bit */
#define SPSR_A			0x00000100 /* System error mask bit */
#define SPSR_D			0x00000200 /* Debug exception mask bit */

/* Give some registers a name */
stack				.req x0
curr_el				.req x1
tmp				.req x2

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

	cmp curr_el, #MODE_EL3t
	ccmp curr_el, #MODE_EL3h, #0x4, ne
	b.eq 3f

	cmp curr_el, #MODE_EL2t
	ccmp curr_el, #MODE_EL2h, #0x4, ne
	b.eq 2f

	cmp curr_el, #MODE_EL1t
	ccmp curr_el, #MODE_EL1h, #0x4, ne
	b.eq 1f

	/* Nothing to do... we are in EL0 */
	b _init

3:
	/* Set EL2 to 64bit */
	mrs tmp, SCR_EL3
	orr tmp, tmp, #(1<<10)		/* Set RW bit */
	msr SCR_EL3, tmp

	bl .LEL3_to_EL2
	/* Now we are in state EL2 */

	/* Empty the pipeline after changing
	 * exception level so that next
	 * instructions are fetched and
	 * executed from the correct state */
	ISB SY				/* Instruction Sync Barrier */

2:
	/* Set little endianess */
	mrs tmp, SCTLR_EL2
	bic tmp, tmp, #(1 << 25)	/* Clear EE bit */
	msr SCTLR_EL2, tmp

	/* Disable EL2 FPU traps */
	mov tmp, #0x33ff
	msr CPTR_EL2, tmp

	/* Set EL1 to 64bit */
	mrs tmp, HCR_EL2
	orr tmp, tmp, #(1<<31)		/* Set RW bit */
	msr HCR_EL2, tmp

	/* Set Stack Pointer */
	msr SP_EL1, stack

	bl .LEL2_to_EL1
	/* Now we are in state EL1 */

	/* Empty the pipeline after changing
	 * exception level so that next
	 * instructions are fetched and
	 * executed from the correct state */
	ISB SY				/* Instruction Sync Barrier */

1:
	/* Set the system control reg for EL1 */
	mov tmp, #0x0800
	movk tmp, #0x30d0, lsl #16
	/* In order to load a 32bit value into tmp two steps are required.
	 * mov loads lower 16bits while movk loads higher 16bits using a
	 * logical left shift of 16 positions.
	 * The assembled value is 0x30d00800 */
	/* This value clears EE and EOE bits setting EL1 in little endian */
	msr SCTLR_EL1, tmp

	/* Disable EL1 FPU traps */
	mov tmp, #(0x3 << 20)
	msr CPACR_EL1, tmp

	/* End of processor initialization */
	b _init
/* END of _reset */

.LEL3_to_EL2:
	/* Set the saved program status reg for EL3 */
	mov tmp, #(SPSR_F|SPSR_I|SPSR_A|SPSR_D|MODE_EL2h)
	msr SPSR_EL3, tmp		/* Drop to EL2 mode with IRQ/FIQ masked */

	/* Set the exception link reg for EL3 */
	adr tmp, _ret
	msr ELR_EL3, tmp		/* Exception Link Register is the register used
                        		 * to jump back to the caller when an exception
                        		 * occurred. Since LR register is not exposed we use
                        		 * a trick to get that value: set ELR_EL3 to a
                        		 * label pointing to a ret instruction. */
	eret				/* This cause to set a new PSTATE, specifically
	    				 * the one written in SPSR_EL3, then returns to
	    				 * the caller */
/* END of .LEL3_to_EL2 */

.LEL2_to_EL1:
	/* Set the saved program status reg for EL2 */
	mov tmp, #(SPSR_F|SPSR_I|SPSR_A|SPSR_D|MODE_EL1h)
	msr SPSR_EL2, tmp		/* Drop to EL1 mode with IRQ/FIQ masked */

	/* Set the exception link reg for EL2 */
	adr tmp, _ret
	msr ELR_EL2, tmp		/* Exception Link Register is the register used
                        		 * to jump back to the caller when an exception
                        		 * occurred. Since LR register is not exposed we use
                        		 * a trick to get that value: set ELR_EL2 to a
                        		 * label pointing to a ret instruction. */
	eret				/* This cause to set a new PSTATE, specifically
	    				 * the one written in SPSR_EL2, then returns to
	    				 * the caller */
/* END of .LEL2_to_EL1 */

	.global _ret

_ret:
	ret
