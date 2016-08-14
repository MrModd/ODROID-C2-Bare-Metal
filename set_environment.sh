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
# These variables should correspond to those on
#get_ready.sh script

CC_NAME="gcc-linaro-aarch64-none-elf-4.9-2014.09_linux"
CC_DIR="$CURR"
CROSS_COMPILE="$CC_DIR/$CC_NAME/bin/aarch64-none-elf-"
#########################################################

if [ "$0" = "$BASH_SOURCE" ] ; then
	echo "You should source this file instead of invoke it directly." >&2
	echo "Use this syntax:" >&2
	echo "source $0" >&2
	exit 1
fi

echo "Setting environment variables..."

"${CROSS_COMPILE}gcc" -v &> /dev/null
if [ $? != 0 ] ; then
	echo "Error setting CROSS_COMPILE variable. Crosscompiler not found." >&2
	return 1
fi

export CROSS_COMPILE="$CROSS_COMPILE"
export CC="gcc"
export CPP="cpp"
export AS="as"
export LD="ld"
export NM="nm"
export OBJCOPY="objcopy"
export OBJDUMP="objdump"

echo
echo "CROSS_COMPILE=$CROSS_COMPILE"
echo "CC=$CC"
echo "CPP=$CPP"
echo "AS=$AS"
echo "LD=$LD"
echo "NM=$NM"
echo "OBJCOPY=$OBJCOPY"
echo "OBJDUMP=$OBJDUMP"
echo
echo "Done."

return 0
