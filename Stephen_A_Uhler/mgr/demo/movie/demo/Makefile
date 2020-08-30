#                        Copyright (c) 1991 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

INSROOT=/usr/mgr
INSDIR = $(INSROOT)/bin
START=.
MAKE=make
CPIO=movie-demo.cpio
FILES= \
	Makefile README fade2intro.Z fadeout.Z fadeout.au \
	part1.Z part1.au part2.Z part2.au script \
	start+.Z start.Z start.au title.Z title.au

all small install clean clobber:

list:
	@for i in ${FILES}; do \
		echo "${START}/$$i";\
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
