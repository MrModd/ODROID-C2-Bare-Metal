#########################################################################
# ODROID C2 Bare Metal                                                  #
# Copyright (C) 2016  Federico "MrModd" Cosentino                       #
#                                                                       #
# This program is free software: you can redistribute it and/or modify  #
# it under the terms of the GNU General Public License as published by  #
# the Free Software Foundation, either version 3 of the License, or     #
# (at your option) any later version.                                   #
#                                                                       #
# This program is distributed in the hope that it will be useful,       #
# but WITHOUT ANY WARRANTY; without even the implied warranty of        #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
# GNU General Public License for more details.                          #
#                                                                       #
# You should have received a copy of the GNU General Public License     #
# along with this program.  If not, see <http://www.gnu.org/licenses/>. #
#########################################################################

ifeq ($(CROSS_COMPILE),)
  $(error You need to export CROSS_COMPILE variable first)
endif

# Where to put the bin file. Should be the same folder defined in
# set_tftp.sh as TFTP_DIR
INSTALLDIR := ../tftp

# Set compilation args
CFLAGS = -Wall -Wextra -Wpedantic -O2 -ffreestanding -std=c99
ARCHFLAGS = -march=armv8-a -mcpu=cortex-a53 -mabi=lp64
ARCHCFLAGS = $(ARCHFLAGS) -mtune=cortex-a53

# Set install invocation
INSTALL = install -m 755

# Find all source files
CFILES := $(shell ls *.c 2>/dev/null)
SFILES := $(shell ls *.S 2>/dev/null)
HFILES := $(shell ls *.h 2>/dev/null)
AOBJS := $(SFILES:%.S=%.o)
COBJS := $(CFILES:%.c=%.o)

# This is the output name for the project
TARGET = program

TAG_SUBDIRS := .
FIND := find
FINDFLAGS := -L



all: $(TARGET).bin $(TARGET).lst $(TARGET).sym

# Generate tag file
tags:
	ctags -w -o tags `$(FIND) $(FINDFLAGS) $(TAG_SUBDIRS) -name '*.[chS]' -print`

# .bin file is extracted from the .elf. It contains just the binary code
%.bin: %.elf
	$(CROSS_COMPILE)$(OBJCOPY) -S -O binary $< $@

# Generate a dump file of the disassembled binary code
%.lst: %.elf
	$(CROSS_COMPILE)$(OBJDUMP) -d $^ > $@

# Generate a list of symbols used in the program
%.sym: %.elf
	$(CROSS_COMPILE)$(NM) $^ | sort > $@

# Install the binary program in the TFTP folder
install: $(TARGET).bin
	$(INSTALL) -d $(INSTALLDIR)
	$(INSTALL) $(TARGET).bin $(INSTALLDIR)



# Generate a compiled program and use the .lds file that defines the structure
# of the elf format
$(TARGET).elf: $(AOBJS) $(COBJS) $(TARGET).lds
	$(CROSS_COMPILE)$(LD) -nostdlib -T $(TARGET).lds -o $@ $(AOBJS) $(COBJS)

$(COBJS): $(HFILES)

# Generate objects file from assembly sources
%.o: %.S
	$(CROSS_COMPILE)$(CPP) $< | $(CROSS_COMPILE)$(AS) $(ARCHFLAGS) -o $@

# Generate objects file from C sources
%.o: %.c 
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(ARCHCFLAGS) -c $<

# Generate assembly code from C sources
%.s: %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(ARCHCFLAGS) -S $<

# Run the preprocessor on the assembly files
%.i: %.S
	$(CROSS_COMPILE)$(CPP) $< > $@

# Run the preprocessor on the c files
%.i: %.c
	$(CROSS_COMPILE)$(CPP) $< > $@



# Don't delete these files if make get killed
.PRECIOUS: %.elf

# These rules don't generate files, execute them every time without checking latest changes
.PHONY: clean

clean:
	$(RM) *~ *.o *.s *.i *.bin *.elf *.lst *.sym tags

