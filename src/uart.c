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

#define uart0_ao_flush() \
		while(!(iomem(UART0_AO_STATUS) & UART0_AO_STATUS_TX_EMPTY))

/**
 * putc: write a character into the UART transmit FIFO
 *
 * @c: the character to write. Note that only least significant
 *     8 bits will be written. Using a 32bit long variable only
 *     for alignment purposes.
 *
 * Returns 1 if the byte was written successfully, 0 if an error
 * occurred.
 */
unsigned int putc(u32 c)
{
	/* Wait if TX FIFO is full */
	while(iomem(UART0_AO_STATUS) & UART0_AO_STATUS_TX_FULL);

	iomem(UART0_AO_WFIFO) |= c & 0xff;

	return (iomem(UART0_AO_STATUS) & UART0_AO_STATUS_ERRORS) ? 0u : 1u;
}

/**
 * puts: print a string on the UART
 *
 * @s: the string to write
 *
 * Returns the number of characters written successfully
 */
unsigned int puts(const char *s)
{
	unsigned int v = 0;
	while (*s) { // Loop until the string ends (character \0)
		v += putc(*s);
		if (*s++ == '\n') {
			v += putc('\r');
		}
	}

	return v;
}

/**
 * puth: print a 32bit unsigned value in hexadecimal
 *
 * @v: the 32bit unsigned value
 *
 * Returns the number of characters written successfully
 */
unsigned int puth(u32 v)
{
	unsigned int w;
	u32 mask;
	u16 i, d;

	w = puts("0x");

	/* Writing from the most significant digit */
	mask = 0xf0000000;
	for (i = 0; mask != 0; mask >>= 4, i +=4) {
		/* Every group of  bits are encoded in a
		 * single hex digit */
		d = (v & mask) >> (28-i);
		w += putc(d + ((d > 9) ? 'a' - 10 : '0'));
	}

	return w;
}

/**
 * puth64: print a 64bit unsigned value in hexadecimal
 *
 * @v: the 64bit unsigned value
 *
 * Returns the number of characters written successfully
 */
unsigned int puth64(u64 v)
{
	unsigned int w;
	u64 mask;
	u16 i, d;

	w = puts("0x");

	/* Writing from the most significant digit */
	mask = 0xf000000000000000;
	for (i = 0; mask != 0; mask >>= 4, i +=4) {
		/* Every group of  bits are encoded in a
		 * single hex digit */
		d = (v & mask) >> (60-i);
		w += putc(d + ((d > 9) ? 'a' - 10 : '0'));
	}

	return w;
}

/**
 * putu: print an unsigned integer value
 *
 * @v: the unsigned value
 *
 * Returns the number of characters written successfully
 */
unsigned int putu(unsigned long v)
{
	/* By dividing by 10 we can get every single digit
	 * of the number, but from the least significant
	 * one. In order to print from the most significant
	 * digit we must use a buffer with the discovered
	 * digits */

	/* a 64bit value can represent numbers up to
	 * (2^64)-1 that fits in Log10((2^64)-1) digits
	 * in base 10 (that is less than 20 digits).
	 * The buffer must also contain the trailing
	 * zero (\0) that ends the string */
	char buff[21];
	unsigned int i;

	if (v < 10) {
		/* Just one digit, no need for buffers */
		return putc(v + '0');
	}

	i = 20;
	buff[i] = '\0';
	while (v != 0) {
		unsigned long w;
		unsigned int r;

		w = v / 10;
		r = v - (w * 10); /* Reminder of the division */

		/* Another way to compute r is with modulus (r = v % 10)
		 * but is computationally more expensive because
		 * it does more divisions */
		
		buff[--i] = (char) (r + '0');
		v = w;
	}

	return puts(buff + i);
}

/**
 * putd: print a signed value
 *
 * @v: the signed value
 *
 * Returns the number of characters written successfully
 */
unsigned int putd(long v)
{
	unsigned int w = 0;

	if (v < 0) {
		w += putc('-');
		v = -v;
	}
	
	w += putu(v);
	return w;
}

/**
 * putf: print a floating point value
 *
 * @v: the signed floating point value
 * @precision: how many digits to print after the decimal dot
 *
 * Returns the number of characters written successfully
 */
unsigned int putf(double v, unsigned int precision)
{
	unsigned int i, w = 0;

	if (v < 0.0) {
		w += putc('-');
		v = -v;
	}

	/* Print integer part (automatic cast) */
	w += putu(v);
	w += putc('.');

	for (i = 0; i < precision; ++i) {
		v = v - (long)v; /* Exclude integer part */
		v *= 10;
		w += putc((long)v + '0');
	}

	return w;
}

/**
 * init_uart: Initialize the UART device and clear its state
 */
void init_uart(void)
{
	u32 baud;

	/* Wait for previous transmissions to end */
	uart0_ao_flush();

	/* Set UART function to the GPIO pinmux */
	iomem_high(AO_REG, AO_REG_UART_AO_A_MASK);

	/* Disable UART */
	iomem_low(UART0_AO_CONTROL, UART0_AO_CONTROL_TX_EN | UART0_AO_CONTROL_RX_EN);

	/* Disable UART FIFO interrupts */
	iomem_low(UART0_AO_CONTROL, UART0_AO_CONTROL_TX_INT_EN | UART0_AO_CONTROL_RX_INT_EN);

	__isb(); /* Instruction barrier */

	/* Set control flags */
	iomem_high(UART0_AO_CONTROL, UART0_AO_CONTROL_TWO_WIRE);
	iomem_low(UART0_AO_CONTROL, UART0_AO_CONTROL_CHAR_LEN_MASK);

	/* Set BAUD rate */
	/* MAGIC AHEAD!
	 * This is not documented on SoC manual.
	 * The following value is taken from the AMLOGIC UART
	 * driver in the Hardkernel repository of the Linux kernel */
	baud = (UART_CLK / 3) / BAUD_RATE - 1;
	baud |= UART0_AO_REG5_USE_BAUD | UART0_AO_REG5_XTAL_CLK;
	iomem(UART0_AO_REG5) = baud;

	/* Reset channels */
	iomem_high(UART0_AO_CONTROL,                   \
			(UART0_AO_CONTROL_RST_TX     | \
			 UART0_AO_CONTROL_RST_RX     | \
			 UART0_AO_CONTROL_CLR_ERROR));

	loop_delay(50u);

	iomem_low(UART0_AO_CONTROL,                    \
			(UART0_AO_CONTROL_RST_TX     | \
			 UART0_AO_CONTROL_RST_RX     | \
			 UART0_AO_CONTROL_CLR_ERROR));

	__isb(); /* Instruction barrier */

	/* Enable UART */
	iomem_high(UART0_AO_CONTROL, UART0_AO_CONTROL_TX_EN | UART0_AO_CONTROL_RX_EN);

	loop_delay(100u);

	putc('\n');putc('\r');
}
