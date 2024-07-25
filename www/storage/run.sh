#!/bin/bash
if [ -z "$1" ]; then
	echo "Usage: $0 <config file name>"
	exit 1
fi

# Set the path to the configuration file
CONFIG_FILE=$1
CONFS_DIR=configs

make
./webserv $CONFS_DIR/$CONFIG_FILE.conf
