#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.1 88/06/21 14:03:05 bianchi Exp $
#	$Source: /tmp/mgrsrc/demo/plot/RCS/Makefile,v $

INSROOT=/usr/mgr
START=.
MGR=../..
MGRLIB=$(MGR)/lib/libmgr.a
MGRINCL=$(MGR)/lib
#
BIN=$(INSROOT)/bin
MAN=$(INSROOT)/man
LIB=$(INSROOT)/lib
CPIO=plot.cpio
CFLAGS=	$(FLAG)  -I$(MGRINCL)
ALL=	libmgrplot.a mgrplot 

CFILES= subs.c mgrplot.c
DOCS= mgrplot.1g
OTHER= Makefile README plot.ex

all small:	$(ALL)

fast:		$(CFILES)
		cc -c $(CFLAGS) $(CFILES)
		make all

libmgrplot.a:	subs.o $(MGRLIB)
		ar r libmgrplot.a subs.o
		ranlib libmgrplot.a

mgrplot:	mgrplot.o libmgrplot.a $(MGRLIB)
		cc -o mgrplot  mgrplot.o libmgrplot.a $(MGRLIB) -lm

install:	$(ALL) $(BIN) $(LIB)
		chmod 755 mgrplot
		rm -f $(BIN)/mgrplot
		cp mgrplot $(BIN)
		rm -f $(LIB)/libmgrplot.a
		cp libmgrplot.a $(LIB)
		ranlib $(LIB)/libmgrplot.a

$(BIN) $(LIB):
		mkdir $@

clean:
		rm -f *.o

clobber:
		rm -f $(ALL)

list:
	@for i in $(CFILES) $(HFILES) $(DOCS) $(OTHER); do \
		echo "$(START)/$$i"; \
	done	

cpio:
	make -s list | cpio -ocv > $(CPIO)
