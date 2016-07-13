# ODROID C2 Bare Metal

## License
Minimal Operating System for ODROID C2 Single-board Computer. \
Copyright (C) 2016 Federico "MrModd" Cosentino

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

## Hardkernel ODROID C2 specifications

![ODROID C2](http://dn.odroid.com/homebackup/201602/ODROID-C2.png "ODROID C2")

**ODROID C2** is an affordable and powerful *Single-Board computer*.
Its characteristics are:

- 64bit Cortex-A53 (ARMv8) quad-core processor @ 2GHz
- 2GB DDR3 Memory @ 912MHz (32bit Bus)
- Gbit Ethernet
- 4 USB 2.0 ports + 1 Micro-USB 2.0 (OTG) port
- Full Type-A HDMI with CEC support
- Infrared receiver
- Serial port
- GPIO, I2C, UART, ADC on main 40 pin header
- I2S on separate header

For a full description of the board visit the [official page] [ODROID-C2].

### Official Wiki

Hardkernel offers a very useful [portal][ODROID portal] for all
the technical documentation of this board. It's an important
resource especially for hardware description and headers pinout.

### Manuals

Essential for this project are the technical manuals for the
board. You will need to understand how to access at very low
level all the devices and how memory and devices are mapped
to the physical addresses. You will also need to access some
ARM co-processors in order to activate some CPU
functionalities such as the floating point unit.

All needed manuals are publicly available for download. Here
is a list of what you'll need:

* [Board schematics]: useful, but not essential; it helps you
                      understand how physical devices are
                      connected to the SoC (such as at which
                      GPIO the System LED is wired);
* [SoC manual]: it describes how to access the peripherals and
                how memory is mapped;
* [Technical Reference Manual]: the main processor manual that
                                describes all its functional
                                aspects (interrups, timers,
                                cache, synchronization primitives,
                                etc.);
* Architecture Reference Manual: it contains the complete instruction
                                 set of the ARM processor. This
                                 manual can be obtained with an
                                 ARM account, but it's not essential.

## Project organization

This repository contains multiple branches; each of which
defines a milestone for the project with an increasing
complexity.

The *master* branch contains just the basic scripts required in order
get all the essential tools:

- **Crosscompiler**: required in order to compile bare-metal
                     programs and bootloader for target architecture;
- **Bootloader**: the first external program the CPU calls, this
                  board uses [U-Boot] as described on the official
                  [Wiki][wiki_uboot] page;
- **TFTP Server**: optional, but very useful to quickly test a new
                   version of the binary program; just compile and
                   reboot the board, U-Boot configuration will instruct
                   to get the file from the network. As TFTP server
                   this project uses [Dnsmasq].

Other branches add at what's already inside the repository folder the
code that should be executed to the board. Subsequent project steps
adds new features to all previous ones.

## How to

This section is work in progress...

[ODROID-C2]: <http://www.hardkernel.com/>
[ODROID portal]: <http://odroid.com/dokuwiki/doku.php?id=en:odroid-c2>
[Board schematics]: <http://dn.odroid.com/S905/Schematic/odroid-c2_rev0.2_20160226.pdf>
[SoC manual]: <http://dn.odroid.com/S905/DataSheet/S905_Public_Datasheet_V1.1.4.pdf>
[Technical Reference Manual]: <http://infocenter.arm.com/help/topic/com.arm.doc.ddi0500g/DDI0500G_cortex_a53_trm.pdf>
[U-Boot]: <http://www.denx.de/wiki/U-Boot>
[wiki_uboot]: <http://odroid.com/dokuwiki/doku.php?id=en:c2_building_u-boot>
[Dnsmasq]: <http://www.thekelleys.org.uk/dnsmasq/doc.html>
