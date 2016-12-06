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

void welcome(void)
{
	puts("\n");
	puts("+--------------------------------------------------+\n");
	puts("|   ___  ____  ____   ___ ___ ____     ____ ____   |\n");
	puts("|  / _ \\|  _ \\|  _ \\ / _ \\_ _|  _ \\   / ___|___ \\  |\n");
	puts("| | | | | | | | |_) | | | | || | | | | |     __) | |\n");
	puts("| | |_| | |_| |  _ <| |_| | || |_| | | |___ / __/  |\n");
	puts("|  \\___/|____/|_| \\_\\\\___/___|____/   \\____|_____| |\n");
	puts("|                                      Bare Metal  |\n");
	puts("|                                                  |\n");
	puts("|  Copyright (c) 2016 Federico \"MrModd\" Cosentino  |\n");
	puts("+--------------------------------------------------+\n");
	puts("\n");
}

void serial_test(void)
{
	unsigned int r;

	puts("=== Serial test ===\n");

	puts("Writing 0xdeadc0de00000000 as 64bit hexadecimal value:\n\t");
	r = puth64(0xdeadc0de00000000);
	puts("\n\tWritten ");putd(r);puts(" characters\n");

	puts("Writing 1099511627776 as unsigned long:\n\t");
	r = putu(0x10000000000);
	puts("\n\tWritten ");putd(r);puts(" characters\n");

	puts("Writing -2345754 as signed long:\n\t");
	r = putd(-2345754l);
	puts("\n\tWritten ");putd(r);puts(" characters\n");

	puts("Writing result of 17/13+3 signed float operation:\n\t");
	r = putf(17.0/13 + 3, 20);
	puts("\n\tWritten ");putd(r);puts(" characters\n");

	puts("====== Done =======\n\n");
}

void main(void)
{
	/* Generate a software interrupt targeting EL1 state */
	//__svc(0);

	/* Print a welcome message */
	welcome();

	/* Print some tests */
	serial_test();

	/* Blinking the SYS_LED indefinitely */

	puts("Start blinking...\n");

	for(;;) {

		/* Set high */
		iomem_high(GPIOAO_OUT, BIT2MASK(GPIOAO_13_OUT_BIT));

		loop_delay(1000000llu);
	
		/* Set low */
		iomem_low(GPIOAO_OUT, BIT2MASK(GPIOAO_13_OUT_BIT));

		loop_delay(1000000llu);
	}
}
