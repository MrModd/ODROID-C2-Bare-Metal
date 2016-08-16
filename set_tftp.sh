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
DNSMASQ_GIT="git://thekelleys.org.uk/dnsmasq.git"
DNSMASQ_COMMIT="master"
DNSMASQ_DIR="$CURR/dnsmasq"
TFTP_DIR="$CURR/tftp"
IFACE="eth0"
IP="10.0.0.1/24"
DNSMASQ_ARGS="--no-daemon --port=0 --enable-tftp --tftp-root=$TFTP_DIR"
#########################################################

# Colors
TXT_COLOR="\e[1;34m" # Normal text
CON_COLOR="\e[1;32m" # Confirm messages
ERR_COLOR="\e[1;31m" # Error messages
RST_COLOR="\e[0m"    # Reset terminal color

#########################################################

# Functions

clone_dnsmasq() {
	echo -e "${TXT_COLOR}Cloning Dnsmasq..${RST_COLOR}"
	
	if [ ! -d "$DNSMASQ_DIR" ] ; then
		git clone $DNSMASQ_GIT -b master "$DNSMASQ_DIR"
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot clone repository.${RST_COLOR}" >&2
			return 1
		fi
		git -C "$DNSMASQ_DIR" checkout $DNSMASQ_COMMIT
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot checkout to commit $DNSMASQ_COMMIT.${RST_COLOR}" >&2
			return 1
		fi
	else
		echo -e "${CON_COLOR}Already present.${RST_COLOR}"
		echo -e "${TXT_COLOR}Pulling last changes...${RST_COLOR}"
		
		git -C "$DNSMASQ_DIR" checkout master
		git -C "$DNSMASQ_DIR" pull
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot pull from origin.${RST_COLOR}" >&2
			return 1
		fi
		git -C "$DNSMASQ_DIR" checkout $DNSMASQ_COMMIT
		if [ $? != 0 ] ; then
			echo -e "${ERR_COLOR}Cannot checkout to commit $DNSMASQ_COMMIT.${RST_COLOR}" >&2
			return 1
		fi
	fi
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
	
	return 0
}

compile_dnsmasq() {
	echo -e "${TXT_COLOR}Compiling Dnsmasq...${RST_COLOR}"
	
	make -C "$DNSMASQ_DIR" -j$(grep -c ^processor /proc/cpuinfo)
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error compiling Dnsmasq.${RST_COLOR}" >&2
		return 1
	fi
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
	
	return 0
}

start_dnsmasq() {
	echo -e "${TXT_COLOR}Trying to set static IP $IP to $IFACE with sudo...${RST_COLOR}"
	
	mkdir -p "$TFTP_DIR"
	
	sudo ip a add $IP dev $IFACE

	echo -e "${TXT_COLOR}Running Dnsmasq with sudo...${RST_COLOR}"
	
	sudo "$DNSMASQ_DIR/src/dnsmasq" $DNSMASQ_ARGS
	if [ $? != 0 ] ; then
		echo -e "${ERR_COLOR}Error running Dnsmasq.${RST_COLOR}" >&2
		return 1
	fi
	
	return $?
}

clean_all() {
	echo -e "${TXT_COLOR}Cleaning all...${RST_COLOR}"
	
	rm -rf "$DNSMASQ_DIR"
	rm -rf "$TFTP_DIR"
	
	echo -e "${CON_COLOR}Done.${RST_COLOR}"
}

SCRIPT_NAME=$0
print_help() {
	echo -e "Usage $SCRIPT_NAME [OPTIONS] [INTERFACE]" >&2
	echo -e "\nOPTIONS:" >&2
	echo -e "\t-h, -?" >&2
	echo -e "\t\tshow this help" >&2
	echo -e "\t-s" >&2
	echo -e "\t\tskip updating if Dnsmasq is already present" >&2
	echo -e "\t-z" >&2
	echo -e "\t\tdelete all downloaded and compiled files" >&2
	echo -e "\nINTERFACE:" >&2
	echo -e "\tTry to set IP address to the given interface." >&2
	echo -e "\tIf not specified $IFACE will be used as default." >&2
}

SKIP_PULL=""

while getopts "h?sz" opt; do
case "$opt" in
	h|\?)
		print_help
		exit 0
		;;
	s)
		SKIP_PULL="true"
		;;
	z)
		clean_all
		exit 0
		;;
esac
done

shift $((OPTIND-1))

if [ $1 ] ; then
	IFACE=$1
fi

if [ ! -d "$DNSMASQ_DIR" ] ; then
	SKIP_PULL=""
fi

if [ ! $SKIP_PULL ] ; then
	clone_dnsmasq
	if [ $? != 0 ] ; then
		exit 1
	fi
fi
compile_dnsmasq
if [ $? != 0 ] ; then
	exit 1
fi
start_dnsmasq
if [ $? != 0 ] ; then
	exit 1
fi

exit 0
