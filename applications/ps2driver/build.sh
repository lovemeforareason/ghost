#!/bin/bash
ROOT="../.."
if [ -f "$ROOT/variables.sh" ]; then
	. "$ROOT/variables.sh"
fi
. "$ROOT/ghost.sh"


# Define build setup
SRC=src
OBJ=obj
ARTIFACT_NAME=ps2driver.bin
CFLAGS="-std=c++11 -I$SRC"
LDFLAGS="-lps2"

# Include application build tasks
. "../applications.sh"
