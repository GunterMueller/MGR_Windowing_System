#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.5 88/08/29 13:38:23 sau Exp $
#	$Source: /tmp/mgrsrc/lib/RCS/Makefile,v $

# makefile for mgr c-interface library

SRC=../src
INSROOT=/usr/mgr
LIB= ./term.o
INSDIR=$(INSROOT)/lib
INCLDIR=$(INSROOT)/include
START=.
CPIO=lib.cpio

# usually passed in from above
#FLAG=-O
CFLAGS=$(FLAG)

PORTCFILES = ckmgrterm.c term.c text.c sfont.c sin.c
CFILES = $(PORTCFILES) scribe.c
OFILES = ckmgrterm.o term.o text.o sfont.o scribe.o sin.o
HFILES = window.h term.h restart.h dump.h


all:	libmgr.a

libmgr.a:	$(OFILES) bitmapread.o bitmaphead.o
		rm -f $@
		ar rv $@ $(OFILES) bitmapread.o bitmaphead.o
		rm -f bitmapread.o bitmaphead.o
		rm -f $(SRC)/bitmapread.o
		rm -f $(SRC)/bitmaphead.o
		ranlib $@

bitmapread.o:
		cd $(SRC); $(MAKE) FLAG=$(FLAG) bitmapread.o
		ln -s $(SRC)/bitmapread.o

bitmaphead.o:
		cd $(SRC); $(MAKE) FLAG=$(FLAG) bitmaphead.o
		ln -s $(SRC)/bitmaphead.o


term.o:		term.h window.h restart.h

text.o:		term.h window.h

textlib.o:	text.o sfont.o scribe.o sin.o 
		ld -r -o textlib.o  text.o sfont.o scribe.o sin.o 

#	the optimizer dies on scribe.c 

scribe.o:	scribe.c
		cc -c scribe.c

smallinstall:	install

install:	libmgr.a $(INSDIR) $(INCLDIR)
	cd $(INCLDIR);  rm -f $(HFILES)
	cp $(HFILES) $(INCLDIR)
#		Do not install the files if INSDIR is the current directory.
#		This stuff with ReMoveThisFile is to get around symbolic links.
	rm -rf ./ReMoveThisFile
	touch $(INSDIR)/ReMoveThisFile
	if [ ! -f ./ReMoveThisFile ]; then \
		(cd $(INSDIR);  rm -f libmgr.a ); \
		cp libmgr.a $(INSDIR); \
	else \
		echo ">>  $(INSDIR) == current directory; no files copied"; \
	fi
	rm -rf $(INSDIR)/ReMoveThisFile

$(INSDIR) $(INCLDIR):
		mkdir $@

clean:	
		rm -f *.o core .done

clobber:
		rm -f $(OFILES) libmgr.a bitmap.h blitlib.a .done

list:
	@for i in Makefile ${CFILES} ${HFILES}; do \
		echo "${START}/$$i"; \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
