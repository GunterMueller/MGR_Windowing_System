#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.1 88/08/24 15:53:38 bianchi Exp $
#	$Source: /tmp/mgrsrc/misc/RCS/Makefile,v $

# utilities stuff for mgr

INCL = ../lib
START=.
CPIO=util.cpio
CFLAGS = -O -I$(INCL)
UTIL_BIN = /usr/local/bin
MANDIR = $(UTIL_BIN)/../man
MGR=../src

CFILES = lasergrafix.c tjfilter.c getscreen.c rotate.c hp_raster.c
ALL = lasergrafix tjfilter getscreen rotate hp_raster
OTHER = \
	Makefile README cshrc login map mgr.ti mgrc plot termcap tree.out mailrc

all:	$(ALL)


getscreen:	getscreen.o 
		cc $(CFLAGS) -o getscreen getscreen.o

lasergrafix:	lasergrafix.o $(MGR)/bitmaphead.o
		cc $(CFLAGS) -o lasergrafix lasergrafix.o $(MGR)/bitmaphead.o

rotate:		rotate.o $(MGR)/bitmaphead.o
		cc $(CFLAGS) -o rotate rotate.o $(MGR)/bitmaphead.o

tjfilter:	tjfilter.o $(MGR)/bitmaphead.o
		cc $(CFLAGS) -o tjfilter tjfilter.o $(MGR)/bitmaphead.o

hp_raster:	hp_raster.o 
		cc $(CFLAGS) -o hp_raster hp_raster.o -lmgr

install:

clean:	
		-rm -f *.o core
clobber:
		-rm -f $(ALL)

list:
	@for i in ${HFILES} ${CFILES} ${OTHER}; do \
	 echo "${START}/$$i"; \
	done	

cpio:
	make -s list | cpio -ov > ${CPIO}

.PHONY:	install clean clobber list cpio
