#!/bin/bash

#set_environment.sh
#Copyright (C) 2016  Federico "MrModd" Cosentino

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

"${CROSS_COMPILE}gcc" -v &> /dev/null
if [ $? != 0 ] ; then
	echo "Error setting CROSS_COMPILE variable. Crosscompiler not found." >&2
	return 1
fi

export CROSS_COMPILE="$CROSS_COMPILE"

echo "CROSS_COMPILE=$CROSS_COMPILE"
echo "DONE."

return 0
