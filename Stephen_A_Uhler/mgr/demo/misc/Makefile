#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.5 88/08/22 10:30:44 sau Exp $
#	$Source: /tmp/mgrsrc/demo/misc/RCS/Makefile,v $

# makefile for demos 

INCL= ../../lib
PORT=../../src/port
LIB= $(INCL)/libmgr.a
BIT= $(PORT)/blitlib.a
CPIO=demo.cpio
TAR = demo.tar
CFLAGS= -I$(INCL) -I$(PORT) $(FLAG)
LDFLAGS=$(LIB)
START=.
CPIO=misc.cpio
INSROOT=/usr/mgr
INSDIR=$(INSROOT)/bin

CFILES= art_data.c bounce.c clock.c clock2.c clock3.c close.c \
	dmgr.c ether.c getpty.c grav.c grid.c hilbert.c hpmgr.c maze.c \
	mgrmail.c mgrmode.c mgrmsgs.c oclose.c omgrmail.c overlayd.c \
	set_console.c set_emacs.c set_termcap.c show.c startup.c \
	stat.c stringart.c  lock.c gtscr.c ptscr.c spot.c

OFILES= art_data.o bounce.o clock.o clock2.o close.o \
	dmgr.o ether.o getpty.o grav.o grid.o hilbert.o hpmgr.o maze.o \
	mgrmail.o mgrmode.o mgrmsgs.o oclose.o omgrmail.o overlayd.o \
	set_console.o set_emacs.o set_termcap.o show.o startup.o \
	stat.o stringart.o lock.o gtscr.o ptscr.o spot.o

SMALL=	bounce clock clock2 close dmgr ether grav grid hilbert \
	hpmgr maze mgrmail mgrmode mgrmsgs spot \
	set_emacs set_termcap show startup stat stringart  getscreen putscreen

ALL = $(SMALL) overlayd set_console lock

OTHER = Makefile

all:		$(ALL)

banner:		banner.o $(LIB)
		$(CC) $(CFLAGS) -o banner banner.o $(LDFLAGS)

spot:		spot.o $(LIB)
		$(CC) $(CFLAGS) -o spot spot.o $(LDFLAGS)

clock:		clock.o $(LIB)
		$(CC) $(CFLAGS) -o clock clock.o $(LDFLAGS)

clock2:		clock2.o $(LIB)
		$(CC) $(CFLAGS) -o clock2 clock2.o $(LDFLAGS)

clock3:		clock3.o $(LIB)
		$(CC) $(CFLAGS) -o clock3 clock3.o $(LDFLAGS)

b2:		b2.o $(LIB)
		$(CC) $(CFLAGS) -o b2 b2.o $(LDFLAGS)

unannotate:		unannotate.o $(LIB)
		$(CC) $(CFLAGS) -o unannotate unannotate.o $(LDFLAGS)

bounce:		bounce.o $(LIB)
annotate:		annotate.o $(LIB)
		$(CC) $(CFLAGS) -o annotate annotate.o $(LDFLAGS)

bounce:		bounce.o $(LIB)
		$(CC) $(CFLAGS) -o bounce bounce.o $(LDFLAGS)

dmgr:		dmgr.o $(LIB)
		$(CC) $(CFLAGS) -o dmgr dmgr.o $(LDFLAGS)

putscreen:		ptscr.o $(LIB) $(BIT) $(OTHERLIB)
		$(CC) $(CFLAGS) -o putscreen ptscr.o $(LDFLAGS) $(BIT) $(OTHERLIB)

getscreen:		gtscr.o $(LIB) $(BIT) $(OTHERLIB)
		$(CC) $(CFLAGS) -o getscreen gtscr.o $(LDFLAGS) $(BIT) $(OTHERLIB)

grav:		grav.o $(LIB)
		$(CC) $(CFLAGS) -o grav grav.o $(LDFLAGS)

hpmgr:		hpmgr.o getpty.o $(LIB)
		$(CC) $(CFLAGS) -o hpmgr hpmgr.o getpty.o $(LDFLAGS)

mgrmsgs:	mgrmsgs.o $(LIB)
		$(CC) $(CFLAGS) -o mgrmsgs mgrmsgs.o $(LDFLAGS)

omgrmail:	omgrmail.o $(LIB)
		$(CC) $(CFLAGS) -o omgrmail omgrmail.o $(LDFLAGS)

mgrmail:	mgrmail.o $(LIB)
		$(CC) $(CFLAGS) -o mgrmail mgrmail.o $(LDFLAGS)

chart:		chart.o $(LIB)
		$(CC) $(CFLAGS) -o chart chart.o $(LDFLAGS)

close:		close.o $(LIB)
		$(CC) $(CFLAGS) -o close close.o $(LDFLAGS)

oclose:		oclose.o $(LIB)
		$(CC) $(CFLAGS) -o oclose oclose.o $(LDFLAGS)

close3:		close3.o $(LIB)
		$(CC) $(CFLAGS) -o close3 close3.o $(LDFLAGS)

stringart:	stringart.o art_data.o $(LIB)
		$(CC) $(CFLAGS) -o stringart stringart.o art_data.o $(LDFLAGS)

stat2:		stat2.o $(LIB)
		$(CC) $(CFLAGS) -o stat2 stat2.o $(LDFLAGS)

stat:		stat.o $(LIB)
		$(CC) $(CFLAGS) -o stat stat.o $(LDFLAGS)

Ether:		Ether.o $(LIB)
		$(CC) $(CFLAGS) -o Ether Ether.o $(LDFLAGS)

ether:		ether.o $(LIB)
		$(CC) $(CFLAGS) -o ether ether.o $(LDFLAGS)

ether2:		ether2.o $(LIB)
		$(CC) $(CFLAGS) -o ether2 ether2.o $(LDFLAGS)

set_console:	set_console.o $(LIB)
		$(CC) $(CFLAGS) -o set_console set_console.o $(LDFLAGS)

set_emacs:	set_emacs.o $(LIB)
		$(CC) $(CFLAGS) -o set_emacs set_emacs.o $(LDFLAGS)

set_termcap:	set_termcap.o $(LIB)
		$(CC) $(CFLAGS) -o set_termcap set_termcap.o $(LDFLAGS)

mgrmode:	mgrmode.o $(LIB)
		$(CC) $(CFLAGS) -o mgrmode mgrmode.o $(LDFLAGS)

show:		show.o $(LIB)
		$(CC) $(CFLAGS) -o show show.o $(LDFLAGS)

startup:	startup.o $(LIB)
		$(CC) $(CFLAGS) -o startup startup.o $(LDFLAGS)

hilbert:	hilbert.o $(LIB)
		$(CC) $(CFLAGS) -o hilbert hilbert.o $(LDFLAGS)

overlayd:	overlayd.o $(LIB)
		$(CC) $(CFLAGS) -o overlayd overlayd.o $(LDFLAGS)

grid:		grid.o $(LIB)
		$(CC) $(CFLAGS) -o grid grid.o $(LDFLAGS)

view:		view.o $(LIB)
		$(CC) $(CFLAGS) -o view view.o $(LDFLAGS)

file:		file.o $(LIB)
		$(CC) $(CFLAGS) -o file file.o $(LDFLAGS)

print:		print.o $(LIB)
		$(CC) $(CFLAGS) -o print print.o $(LDFLAGS)

wprint:		wprint.o $(LIB)
		$(CC) $(CFLAGS) -o wprint wprint.o $(LDFLAGS)

lock:		lock.o $(LIB) $(BIT)
		$(CC) $(CFLAGS) -o lock lock.o $(BIT) $(OTHERLIB)

maze:		maze.o $(LIB)
		$(CC) $(CFLAGS) -o maze maze.o $(LDFLAGS)

$(PORT):
		(cd $(PORT); $(MAKE) blitlib.a)

$(LIB):
		(cd $(INCL); $(MAKE) libmgr.a)

$(OFILES):	../../lib/term.h $(LIB)

install:	$(ALL) $(INSDIR) $(INSDIR/old)
		-cd $(INSDIR);  mv -f $(ALL) old
		cp $(ALL) $(INSDIR)

$(INSDIR) $(INSDIR)/old:
	mkdir $@

tar:		   
		tar cf $(TAR) `ls $(CFILES) $(SH) $(OTHER)`

clean:		
		rm -f *.o

clobber:		
		rm -f $(ALL)

list:
	@for i in ${CFILES} ${HFILES} ${OTHER}; do \
		echo "${START}/$$i"; \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
