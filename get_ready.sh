#!/bin/bash

##########################################################################
# ODROID C2 Bare Metal
# Copyright (C) 2016  Federico "MrModd" Cosentino
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
##########################################################################

CURR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd )" # Current directory

#########################################################
# Variables
#########################################################
CC_NAME="gcc-linaro-aarch64-none-elf-4.9-2014.09_linux"
CC_URLS=("https://releases.linaro.org/14.09/components/toolchain/binaries/gcc-linaro-aarch64-none-elf-4.9-2014.09_linux.tar.xz" \
			"http://dn.odroid.com/toolchains/gcc-linaro-aarch64-none-elf-4.9-2014.09_linux.tar.xz")
UBOOT_GIT="https://github.com/hardkernel/u-boot.git"
# Last commit (61f29bb) in branch odroidc2-v2015.01 breaks compilation.
# Waiting for a patch reverting to a previous working commit.
UBOOT_COMMIT="2fcbc68"
#UBOOT_COMMIT="odroidc2-v2015.01"
BOOT_FILES=("https://github.com/hardkernel/u-boot/raw/odroidc2-v2015.01/sd_fuse/bl1.bin.hardkernel" \
			"https://github.com/hardkernel/u-boot/raw/odroidc2-v2015.01/sd_fuse/sd_fusing.sh")
UBOOT_DIR="$CURR/uboot"
UBOOT_BIN="$UBOOT_DIR/sd_fuse/u-boot.bin"
CC_DIR="$CURR"
CROSS_COMPILE="$CC_DIR/$CC_NAME/bin/aarch64-none-elf-"
BOOT_DIR="$CURR/sdcard"
#########################################################

# Get branch that contains a commit:
#   git branch --contains <commit>
#
# Get the commit linked to the given tag:
#   git rev-parse <tag>~0

# Colors
TXT_COLOR="\e[1;34m" # Normal text
CON_COLOR="\e[1;32m" # Confirm messages
ERR_COLOR="\e[1;31m" # Error messages
RST_COLOR="\e[0m"    # Reset terminal color

#########################################################

unset LD_LIBRARY_PATH

# Functions

download_crosscompiler() {
	echo -e "${TXT_COLOR}Downloading crosscompiler...${RST_COLOR}"
	
	if [ ! -d "$CC_DIR/$CC_NAME" ] ; then
		found="false"
		file=""
		for url in "${CC_URLS[@]}" ; do
		
			echo -e "${TXT_COLOR}Trying${RST_COLOR} \"$url\""
			
			wget $url -P "/tmp"
			if [ $? == 0 ] ; then
				echo -e "${TXT_COLOR}File found!${RST_COLOR}"
				
				file=$(echo $url | sed 's/.*\///')
				found="true"
				
				break
			fi
		done
		if [ "$found" != "true" ] ; then
			echo -e "${ERR_COLOR}Cannot download crosscompiler.${RST_COLOR}" >&2
			return 1
		fi
		tar -xf "/tmp/$file" -C "$CC_DIR"
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot extract crosscompiler.${RST_COLOR}" >&2
			return 1
		fi
		
		rm -f "/tmp/$file"
		
		echo -e "${CON_COLOR}Done.${RST_COLOR}"
	else
		echo -e "${CON_COLOR}Already present.${RST_COLOR}"
	fi
	
	return 0
}

clone_uboot() {
	echo -e "${TXT_COLOR}Cloning U-boot...${RST_COLOR}"
	
	if [ ! -d "$UBOOT_DIR" ] ; then
		git clone $UBOOT_GIT -b master "$UBOOT_DIR"
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot clone repository.${RST_COLOR}" >&2
			return 1
		fi
		git -C "$UBOOT_DIR" checkout $UBOOT_COMMIT
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot checkout to commit $UBOOT_COMMIT.${RST_COLOR}" >&2
			return 1
		fi
	else
		echo -e "${CON_COLOR}Already present.${RST_COLOR}"
		echo -e "${TXT_COLOR}Pulling last changes...${RST_COLOR}"
		
		git -C "$UBOOT_DIR" checkout master
		git -C "$UBOOT_DIR" pull
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot pull from origin.${RST_COLOR}" >&2
			return 1
		fi
		git -C "$UBOOT_DIR" checkout $UBOOT_COMMIT
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot checkout to commit $UBOOT_COMMIT.${RST_COLOR}" >&2
			return 1
		fi
	fi
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
	
	return 0
}

compile_uboot() {
	echo -e "${TXT_COLOR}Compiling U-boot...${RST_COLOR}"
	
	export ARCH="arm"
	export CROSS_COMPILE="$CROSS_COMPILE"
	export PATH="$CC_DIR/$CC_NAME/bin/:$PATH"
	
	make -C "$UBOOT_DIR" odroidc2_config
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error setting U-boot configuration.${RST_COLOR}" >&2
		return 1
	fi
	
	make -C "$UBOOT_DIR" -j$(grep -c ^processor /proc/cpuinfo)
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error compiling U-boot.${RST_COLOR}" >&2
		return 1
	fi
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
	
	return 0
}

copy_files() {
	echo -e "${TXT_COLOR}Copying files for booting...${RST_COLOR}"
	
	mkdir -p "$BOOT_DIR"
	
	echo -e "${TXT_COLOR}U-boot binary...${RST_COLOR}"
	if [ -e "$UBOOT_BIN" ] ; then
		file=$(echo $UBOOT_BIN | sed 's/.*\///')
		if [ -e "$BOOT_DIR/$file" ] ; then
			echo -e "${CON_COLOR}Already exists.${RST_COLOR}"
		else
			cp "$UBOOT_BIN" "$BOOT_DIR"
			if [ $? != 0 ] ; then
				echo -e "${ERR_COLOR}Error copying U-boot binary.${RST_COLOR}" >&2
				return 1
			fi
			echo -e "${CON_COLOR}Done.${RST_COLOR}"
		fi
	else
		echo -e "${ERR_COLOR}U-boot binary not found, you should compile it first. Continuing anyway...${RST_COLOR}" >&2
	fi
	
	for url in "${BOOT_FILES[@]}" ; do
		file=$(echo $url | sed 's/.*\///')
		echo -e "${TXT_COLOR}$file...${RST_COLOR}"
		if [ -e "$BOOT_DIR/$file" ] ; then
			echo -e "${CON_COLOR}Already exists.${RST_COLOR}"
		else
			wget $url -P "$BOOT_DIR"
			if [ $? != 0 ] ; then
				echo -e "${ERR_COLOR}Error downloading $file. Continuing anyway...${RST_COLOR}" >&2
			fi
			if [ "${file##*.}" == "sh" ] ; then
				chmod +x "$BOOT_DIR/$file"
			fi
			echo -e "${CON_COLOR}Done.${RST_COLOR}"
		fi
	done
	
	echo -e "${TXT_COLOR}README...${RST_COLOR}"
	cp "$CURR/configs/final_readme" "$BOOT_DIR/README"
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error copying README file.${RST_COLOR}" >&2
	fi
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
	
	echo -e "${TXT_COLOR}boot.ini...${RST_COLOR}"
	cp "$CURR/configs/boot.ini" "$BOOT_DIR/boot.ini"
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error copying boot.ini file.${RST_COLOR}" >&2
	fi
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
}

clean_all() {
	echo -e "${TXT_COLOR}Cleaning all...${RST_COLOR}"
	
	rm -rf "$BOOT_DIR"
	rm -rf "$UBOOT_DIR"
	rm -rf "$CC_DIR/$CC_NAME"
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
}

SCRIPT_NAME=$0
print_help() {
	echo -e "Usage $SCRIPT_NAME [OPTIONS]" >&2
	echo -e "\nOPTIONS:" >&2
	echo -e "\t-h, -?" >&2
	echo -e "\t\tshow this help" >&2
	echo -e "\t-c" >&2
	echo -e "\t\tdownload the crosscompiler" >&2
	echo -e "\t-u" >&2
	echo -e "\t\tclone and compile U-boot" >&2
	echo -e "\t-p" >&2
	echo -e "\t\tcopy boot files" >&2
	echo -e "\t-z" >&2
	echo -e "\t\tdelete all downloaded and compiled files" >&2
}

# Parse command line arguments

ARG_CC=""
ARG_UBOOT=""
ARG_COPY=""
ARG_CLEAN=""

while getopts "h?cupz" opt; do
case "$opt" in
	h|\?)
		print_help
		exit 0
		;;
	c)
		ARG_CC="true"
		;;
	u)
		ARG_UBOOT="true"
		;;
	p)
		ARG_COPY="true"
		;;
	z)
		ARG_CLEAN="true"
		;;
esac
done

if [ $ARG_CLEAN ] && ( [ $ARG_CC ] || [ $ARG_UBOOT ] || [ $ARG_COPY ] ) ; then
	echo -e "${ERR_COLOR}Clean must be done alone.${RST_COLOR}" >&2
	exit 1
fi

if [ ! $ARG_CLEAN ] && [ ! $ARG_CC ] && [ ! $ARG_UBOOT ] && [ ! $ARG_COPY ] ; then
	echo -e "${ERR_COLOR}Nothing to do.${RST_COLOR}" >&2
	print_help
	exit 1
fi

shift $((OPTIND-1))
#[ "$1" = "--" ] && shift

# Do stuff

if [ $ARG_CC ] ; then
	download_crosscompiler
	if [ $? != 0 ] ; then
		exit 1
	fi
fi

if [ $ARG_UBOOT ] ; then
	clone_uboot
	if [ $? != 0 ] ; then
		exit 1
	fi
	compile_uboot
	if [ $? != 0 ] ; then
		exit 1
	fi
fi

if [ $ARG_COPY ] ; then
	copy_files
	if [ $? != 0 ] ; then
		exit 1
	fi
fi

if [ $ARG_CLEAN ] ; then
	clean_all
	if [ $? != 0 ] ; then
		exit 1
	fi
fi

exit 0

