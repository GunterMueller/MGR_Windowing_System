#!/bin/sh

# Play an mgr movie (SAU)

# set HERE to the path of this directory (path name should start with /)
HERE=XXXX

export SHELL PATH MGRSIZE
MGR=mgr
SHELL=/bin/sh
MGRSIZE="0 0 1152 64"

PATH=$HERE:/usr/ucb:/bin
# get default script

script=${1:-script}

#	create MGR startup file

TMP=/tmp/movie.$$
rm -f $TMP

sed <<END -e 's+SCRIPT+`cat '$script'`+' > $TMP
map 0 1 2 3 0 5 6 7
window 100      7       400     46
        start set_console\r
window 966      1       63      63
        start clock2\r
window 1        1       950     63
        start vcr SCRIPT | play_data -v -y64 | play_audio >&- 2>&-\r
done
END

clear
echo "Press CR to start, diamond-shift-Q to exit"

# start mgr
$MGR -s $TMP
rm -f $TMP
exit 0
