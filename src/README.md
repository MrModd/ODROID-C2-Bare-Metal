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
This file for now initializes only the .bss section of the program,
but it will be used also for other purposes. Length of .bss section
is obtained by the linker script *program.lds* that defined two
labels, *_bss_start* and *_bss_end* at the beginning and at the
end of the section.

### loop_delay() function

This is the first version of a delay function. It is necessary in
order to wait from a state of the LED and another, but at this
time it is inaccurate. Basically it counts until the number specified
as argument and the speed of this operation depends mostly on
the CPU frequency.

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
