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
translated into AARCH64 assembly, a linker script that describe how to
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