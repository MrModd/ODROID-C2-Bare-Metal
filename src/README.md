# Code description

## Boot process

As the AMLOGIC SoC manual describes it, the boot process depends on some
hardware lines. Based on how they are wired they force a different boot
order. Generally, software side, what happens is that after releasing
the reset state of the processor it starts executing instructions from
a fixed address. Where this address is it depends on the values of the
hardware lines and on the SoC architecture. In most cases processor
execution starts from a small ROM inside the SoC that initializes an
external memory (eg. NAND, SD or eMMC) and then jumps to an address on
that device.

In the case of the ODROID C2 this address is the starting point of U-Boot,
the second stage of the booting process. After U-Boot ended its init
process it starts looking for the final program to load in RAM and it
jumps into it. This will run until the CPU will shutdown. This program
can be for example the Linux kernel, but in the case of this bare-metal
project it will be the generated binary file.

An important consideration that must be done at this point is that all
the addresses contained in the bare-metal program are absolute and they
are not relocable, so that the address linked to the first instruction
must be also the physical memory address on which the instruction itself
is stored. This argument will be resumed talking about the linker script.

The U-Boot config file, *boot.ini*, that you shoud have put in the
microSD card contains directions for the bootloader on how to retrieve the
program. There are two steps: first load the program into memory, then
jump into it with an unconditional branch. In the config file it's set to
get the file *program.bin* from a TFTP server and there's also a
commented line that alternativelly gets the file from the FAT partition
of the microSD. After executing this command the program shoud be in RAM
and the *go* command of U-Boot specified on the config file should make
the bootloader jump into it.

## Compile and install

To compile the program prepare the shell with the **set_environment.sh**
script and type **make** on this directory.
You'll need to copy the binary file in the TFTP root folder so that U-Boot
will get the latest version of the program. Use **make install** in order
to copy the file on the correct location. Note that this command will also
compile the program if necessary.

## 01 - Noop

For this step the important is to generate some code that can run on the
processor and possibly test if it's really running.

Just three files are needed: a source file containing the C code to be
translated into ARMv8-A assembly, a linker script that describe how to
assemble the binary and a makefile that automate the build process.

### main.c

The source file contains just an infinite for loop that will be translated
in a single assembly instruction: a branch to itself. At the end of the
compilation a .lst file should have been generated with the disassebled
version of the binary program. Verify that it contains just a *b*
instruction and that the destination address is also the address of the
same instruction. Moreover this address shoud be the one defined as
*mem_start* in the linker script *program.lds*.

### program.lds

In the linker script it is described the order in which the sections
must be written on the binary ELF file. This is important in order to
make sure that the first function to be executed, specifically *main()*
with the infinite loop, is put at the beginning of the binary. Its
address will correspond to the base memory address where the file was
loaded, so when you jump to the program from the bootloader it will be
the first instruction to be executed.
As already said the address on which the program is loaded must equal
that written on the binary. If for example the program is loaded by the
bootloader on address 0x1100000, this must be the initial address of the
*.text* section in program.lds. In this way the *main()* function will be
put exactly at that address and when the bootloader will jump on the
first address of the loaded program it will execute exactly *main()*.

### Makefile

This is where the magic happens. From the source files object files are
produced and contain instructions readable by the target architecture.
In the specific case of the Cortex-A53 this is an ARMv8-A ISA.
The architecture is specified with the *-march*, *-mtune* and *-mcpu*
flags during the compilation. Object files are then linked together in
an ELF file that contains the binary program. With *objdump* the binary
instructions are extracted from the file and put in another file. This
is the *.bin* file that will be loaded into the memory.

During execution the program won't have the C standard library because
it is bare-metal. The *-ffreestanding* and *-nostdlib* flags prevent
linkage toward dynamic libraries.

### Testing

In order to test whether or not the written code is working you should
verify that the branch instruction is running in the first CPU of the
board. This is not possible, but you can tell if the program it's running
by looking at the disassembled version of the binary and the bootloader
errors.
As said the binary file should contains just an instruction, an
unconditional branch to itself. You can see this from the *program.lst*
file. Second, the binary file should have the dimension of a single
instruction that is 4 bytes, not a byte more and not a byte less.
Finally if you load the program from U-Boot and you jump at the specified
address you shouldn't see anythig new after the *go* command. If the
program was incorrect you probably would have seen an exception managed
by U-Boot.

## 02 - Led

A feedback is needed in order to test if program is really running on
the board. The simplest thing to do is to change the state of a GPIO
wired to a LED and check whether the light goes on or off.
ODROID C2 has one LED wired to the bank AO of the GPIO controller.
From the schematics of the board you can understand at which line it
is wired. It results that the blue LED is wired to the *SYS_LED* signal
that reach the SoC through the **GPIOAO_13** line. Note that an **Low**
level to the GPIO turns the LED on, because SYS_LED signal is wired to
the cathode.

The board powers up with the blue LED turned on. So the purpose of
this part is to turn it off setting an **High** level to the GPIO
register.

Generally there are several registers related to each GPIO:

- **PINMUX**: these registers define the purpose of the SoC pin in the
              case that different functions are associated to the same
              pin;
- **CONTROL**: these registers define the direction of the GPIO, the
               value read from the port in case of Input or written
               to the latch in case of Output and the Pull-up/down.

These registers have to be found on the SoC manual.

The *odroidc2.h* header file should be used as general definition file
for the board. This loads all other header files organized by subsystem.
*odroidc2_gpio.h* contains all the definitions related to the GPIOs
as well as all the bitmask needed in order to change the state of the
LED.

### Testing

This is a simple task: run the program and verify that the blue LED turns
off. If it doesn't try checking first all the addresses you used and then
read the assembly code in *program.lst* file looking for strange behaviour.

## 03 - Blink

For this step the important is to put the CPU in a known state and
initialize the stack pointer so that function calls can work.

ARMv8-A introduces two **execution states**: Aarch32 and Aarch64. On
the first one you can use A32 or T32 instruction sets, mostly
compatible with ARMv7-A architecture, On the second one there's the A64
instruction set with 32bit instructions and 64bit registers.

Hierarchical protection domains (ring level) are guaranteed in Aarch64
by 4 **exception level**: EL3, EL2, EL1, EL0. Where EL3 is the most
privileged and EL0 the least privileged. Generally, registers whose
name end with *_ELx* means that can be accessed from level ELx and
superior. Aarch32 keeps the privilege definition of ARMv7-A.

Passing from one execution state to the other or from an exception level
to another one must be done on an exception boundary. EL0 is used for
user applications, EL1 for the OS (supervisor), EL2 (eventually) for the
hypervisor and finally EL3 for secure applications.

### startup.S

The goal is to determine the current execution level and then switch to
EL1 in Aarch64. This can be done by setting desired state on some special
registers and then cause an exception return (with ERET instruction).
The file *startup.S* do exactly this. It reads current state from
special register "CurrentEL" and then jumps to the right label. If in
EL3 it sets EL2 in Aarch64 and then drop to EL2. When in EL2 (because
of the drop or because it started in this exception level) it sets
the endianess and the FPU traps for the same EL and it prepares the
execution state for EL1. At this point the **stack pointer** is
initialized for EL1 and set to the address at the end of the memory
space. This address was defined in the linker script file *program.lds*.
Then privileges are dropped again from EL2 to EL1 and finally the
system control register for this execution level is initialized.

At this point, unless this code was first executed from EL0 (but
it won't happen if invoked from the bootloader), the CPU is in
the exception level EL1. If we started from EL2 or EL3 we are also
sure that the current execution state is Aarch64.

### init.c

After *startup.S* execution jumps to function *_init()* in *init.c*.
This file for now initializes only the .bss section of the program
and the GPIO, but it will be used also for other purposes.
Length of .bss section is obtained by the linker script *program.lds*
that defined two labels, *_bss_start* and *_bss_end* at the beginning
and at the end of the section.

### loop_delay() function

This is the first version of a delay function. It is necessary in
order to wait from a state of the LED and another, but at this
time it is inaccurate. Basically it counts from zero to the number
specified as argument and the speed of this operation depends
mostly on the CPU frequency.

### Testing

Running the program should now cause the LED to blink. A useful
counter test can be to define the "stack_top" label in *program.lds*
to a value out of the memory space, for example 3GB, and see
if without a correct stack the program would work.
If this test fails, meaning that the LED doesn't blink, it tells
you two things:

- the program was invoked from an exception level greater than
  EL1, because the stack pointer is set at EL2;
- the drop of the privileges worked fine.

## 04 - Memory Barrier

A Memory Barrier is an instruction that force an ordering constraint
on memory accesses. This is sometime necessary due to CPU instruction
prefetching and/or pipeline.

As example consider two instruction that access the memory: a store
operation followed by a read operation on the same memory location.
Because of the pipeline the read instruction is executed before
the ending of the store operation. This means that probably the
second instruction reads a different value respect of what the
store instruction wrote. A memory barrier prevents this from
happening forcing the CPU to delay the read instruction until
all the store operations preceding the barrier end.

There are also variations of the memory barrier just described.
They differs on which instructions must be delayed and which
instructions can cause a delay. For example you can put a memory
barrier just for read accesses after a write access or for both
reads and writes after a write access. In ARMv8 these variations
are specified by the argument taken by the assembly instruction
that implements the barrier.

Memory barrier is not the only kind of barriers that can be
used. A Data Synchronization Barrier extends the concept of
memory barrier also to all instructions that follow the
barrier. CPU stalls until all memory accesses are completed.
An Instruction Synchronization Barrier, instead, forces the
CPU to discard all the instructions already on the pipeline.

At this stage of development a memory barrier is used to speedup
the execution of the delay function because a barrier is
generally faster than a store in memory. In file "common.h" the
assignment used in the loop, used to prevent a compiler optimization,
is substituted by a memory barrier.

Moreover an instruction synchronization barrier should be
used after changing the exception level so that every
instruction that goes on the CPU after that executes from
the new exception level. You can find this barrier in
"startup.S" after calling the two labels ".LEL2_to_EL1"
and ".LEL3_to_EL2".

### Testing

Due to the faster speed of execution of the memory barrier in
respect of the memory assignment the LED shoud blink faster.
Moreover the produced assembly should have less instructions
for the *loop_delay()* function. In fact the volatile modifier
for the variable tells the compiler that the value could change
in memory even if it is never used from the code. This forces
the compiler to take its vaule from memory every time it is
accessed and store it in memory after every write.
The assign operation in the while loop is therefore always
translated in a read from memory, an update of its value and
a store. Three instructions that are substituted by a single
barrier instruction.

## 05 - iomemdef

For now, setting the GPIO output level requires at least one
read from memory that gets the value contained in the variable
(that is the address of the GPIO register) and one read from
the I/O device that gets the actual state of the GPIO.
Let's assume that you have the following variable:

```c
volatile u32 *gpio1 = (u32 *) 0x12345678;
```

You want to update the value of the register pointed by the
I/O address *0x12345678*. First of all you need to obtain this
address reading from memory the variable *gpio1*. After that
you can load in a CPU register the value contained at that
address.

This first access can be avoided if you put the address value
directly as a constant that will be translated to an immediate
value. This is done by the **iomemdef()** macro that
generates at compile time new constants. All these addresses
can be accessed as offset of a base address defined under the
name of **_iomem**. The macro **iomem()* helps accessing
the address space as if it was an array. To make this
mechanism more efficient it's important to set _iomem to
zero, because in this way there's no need to compute the
final address *_iomem[n]* (where n is a constant generated
with iomemdef()) because 0 + n is always equal to n and can
be simplyfied at compile time.

Keep in mind that even if this is a 64bit architecture, as
the SoC manual says peripheral registers are all 32bit long. For
that reason this iomem uses 32bit variables.

### Testing

In the compiled version of the program (program.lst) you should
see less load operations.

## 06 - exceptions

When an interrupt occurs the execution must jump to a routine
that can handle this event. Normal flow must be interrupted
and then restored from the last instruction that went on the CPU.
After registering the interrupt the execution jumps to a
well known address that can be configured through a special
CPU register, the **Vector Base Address Register** (VBAR).
This address represent the beginning of the Exception Table
that contains the instructions that must be executed for the
event handling. Several types of exceptions can be caught and
a different section of this table is used for each of them.

There are 4 types of exceptions: **synchronous**, **IRQ**,
**FIQ** and **asynchronous**. For each of them there're 4
entries in the table. The one called when the event occurrs
depends on the CPU state:

- CPU was already at the Exception Level used to handle
  the exception and SP0 is used as Stack Pointer for each
  EL;
- CPU was already at the Execution Level used to handle
  the exception and each EL uses a different Stack Pointer;
- CPU was in a lower Exception Level and in AArch64
  execution state;
- CPU was in a lower Exception Level and in AArch32
  execution state.

In ARMv8 a software interrupt can be generated with **SVC**
or **HVC** instructions. First one generates a synchronous
exception targeting EL2, second one generates the same
exception targeting EL3.

### Testing

For now the important is to verify the correctness of the
exception table, including offsets and base address.
Executing **SVC** instruction after initializing the
VBAR address should jump to the entry of the table
related to a synchronous exception targeting the same
EL of the normal flow of execution and with a different
SP for each EL. In short it should execute the instruction
at offset 0x200 from the exception base address.

## 07 - UART

The only way to understand what's happening on the board
is through the system LED. It's time to pass to a more
complete feedback like the serial port.

The ODROID C2 is equipped with several UART (Universal
Asynchronous Receiver/Transmitter) one of these is wired
to the *serial console port*, the white 4 pin connector
on the opposite side of the USB ports.

### Terminal emulation

Although a serial device can be accessed directly from
its *device file* in the /dev virtual filesystem, it's
useful to adopt a terminal emulator such as **Minicom**.

In order to "see" on the screen what the serial device
is transmitting it's essential to configure the terminal
emulator with the same configuration of the device.
Even if this can sound a futile reminder it's not always
so immediate to understand how the hardware device should
be set up or what's its state after a hard reset of the
board.

The configuration we'll use for this UART is:

- Two wire configuration (No CTS/RTS signals)
- 8 bit per character
- No parity bit
- No stop bit
- Baud rate 115200 bps

### Wiring

Next step, after configuring the terminal emulation, is
to understand how the serial port on the ODROID C2 is
wired to the SoC.

From the board schematics CON5 (the white connector)
has the following pinout:

```
________UART_________
|Pin 4 - GND        |
|Pin 3 - TTA_TXD    |
|Pin 2 - TTA_RXD    |
|Pin 1 - VDDIO_AO3V3|
\___________________|

```

Where **TTA_RXD** is directly connected to **GPIOAO_0**
(alias **UART_TX_AO**) and **TTA_TXD** to **GPIOAO_1**
(**UART_RX_AO**).

These are general purpose registers that means you need
to correctly set the pinmux first in order to expose the
serial signal on the ports instead of the GPIO.

As mentioned before a **pinmux** is a signal multiplexer that
let to make available more devices on the same pins. The
downside is that you cannot use all of them at the
same time. For example the mentioned two lines are
wired to the serial device and can also be used as GPIO,
but not at the same time.

### Registers

Generally there are at least 4 registers that helps to
access an UART device:

- A setup register, that is required to set and initialize
  the device;
- A status register, that shows the current status of the
  read and write buffers as well as the activity on the
  TX and RX ports;
- A read register, that contains the data received;
- A write register, where you store the data that the
  device will send on the serial channel.

These registers and their descriptions can be found on the
SoC manual of the board. On the AMLOGIC S905 (the ODROID C2
SoC) there are 6 registers for each UART device built in
the chip. In addition to the 4 mentioned there are the
UART IRQ control register and a register for the Baud rate
configuration.

### Port configuration

There are few essential step to follow in order to set the
device in a known and working state. First of all (as said)
you need to expose the UART to the pinmux, second you should
set the desired mode and speed of the serial port and
third you must enable (or turn on) the device that on this
board consist in enabling the TX and RX channels.

On some devices you also need to start the clock source that
provides the synchronization signal to the UART, but in this
case we'll use an external crystal oscillator (XTAL_CLK).

### Print functions

After initializing the UART device it's useful to abstract
the writing process on the serial channel.

Sending a character consist of writing the bits on the
write register. The device will take them and put (if present)
in the output FIFO until it will be sent.

Sending single characters or strings is pretty easy because
they are already sequence ASCII symbols that can be printed
on the screen of the receiver. On the contrary numbers must be
converted from their binary representation to printable
strings.

Starting from a function that sends a single ASCII character
you can build more complex functions that character by
character send a string or parse a number and send its
decimal representation. At the end you always need to
print a sequence of ASCII character in order to see
the number on the receiver terminal.

These functions should include the possibility to print
signed and unsigned integers, decimal numbers and hexadecimal
numbers that can be useful to prepresent addresses.

On the ODROID C2 you should be careful on the length of
the variables because some values can be 64bit long.

### Testing

Now you have a better way to debug the board and understand
what's happening. Strings can be printed at some points
on the code in order to understand when (or if) it is
reached.

For example the code in this branch should print the
following splash screen:

```
+--------------------------------------------------+
|   ___  ____  ____   ___ ___ ____     ____ ____   |
|  / _ \|  _ \|  _ \ / _ \_ _|  _ \   / ___|___ \  |
| | | | | | | | |_) | | | | || | | | | |     __) | |
| | |_| | |_| |  _ <| |_| | || |_| | | |___ / __/  |
|  \___/|____/|_| \_\\___/___|____/   \____|_____| |
|                                      Bare Metal  |
|                                                  |
|  Copyright (c) 2016 Federico "MrModd" Cosentino  |
+--------------------------------------------------+

=== Serial test ===
Writing 0xdeadc0de00000000 as 64bit hexadecimal value:
        0xdeadc0de00000000
        Written 18 characters
Writing 1099511627776 as unsigned long:
        1099511627776
        Written 13 characters
Writing -2345754 as signed long:
        -2345754
        Written 8 characters
Writing result of 17/13+3 signed float operation:
        4.30769230769230748734
        Written 22 characters
====== Done =======

Start blinking...
```
