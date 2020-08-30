#!/bin/sh

# make an mgr movie  - sample script (SAU)

export PATH GAIN
GAIN=125
MGR=mgr		#	Must be a version that supports scripting
BIN=XXXX	#	must be a bin where all the stuff resides
PATH=$BIN:$PATH

log=${1:-/tmp/$USER}
diag=/tmp/log.$$
if [ x$1 != x ]
then
	shift
fi

clear
echo "Press diamond-S to start video logging"
echo "Press diamond-s to stop video logging"
echo "Log files will start with $log"
echo "scripting log will go to $diag"
echo "Press CR to start"
read foo

# start mgr

$MGR -dL -z "record $log" $* 2>$diag
rm -f $diag
exit 0
