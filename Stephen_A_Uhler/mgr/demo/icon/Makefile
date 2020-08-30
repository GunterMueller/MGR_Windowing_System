#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.14 88/08/24 16:00:08 bianchi Exp $
#	$Source: /tmp/mgrsrc/demo/icon/RCS/Makefile,v $

# icon stuff
#
#	walk,mclock,cycle  courtesy  S. D. Hawley
#	squeeze, unsqueeze courtesy of M. E. Lesk
#
MGR=../..
INCL = $(MGR)/lib

#	need pixrect if mgr is pixrect (or color) version
#	zoom and snap don't work in color, see NOCOLOR= below

FLAG=-g
LIB = $(MGR)/lib
LIBMGR = $(LIB)/libmgr.a
BITDIR=$(MGR)/src/port
BITLIB = $(BITDIR)/blitlib.a $(OTHERLIB)
CFLAGS= -I$(INCL) -I$(BITDIR) $(FLAG) -I/usr/local/include

CFILES= \
	browse.c chess.c cut.c cycle.c \
	iconmail.c iconmsgs.c icontoc.c invert_colormap.c \
	mclock.c overlay.c pilot.c set_colormap.c snap.c \
	walk.c window_print.c zoom.c  squeeze.c unsqueeze.c \
	pgs.c

HFILES = mail_icons.h msgs_icons.h
OTHER = Makefile README
START=.
CPIO=demo_icon.cpio
INSROOT=/usr/mgr
INSDIR=$(INSROOT)/bin

ALL= \
	browse chess cycle cut iconmail iconmsgs mclock \
	walk window_print cycle squeeze unsqueeze pilot \
	set_colormap overlay invert_colormap \
	zoom snap

all:	$(ALL)

convert_icon:	convert_icon.o $(LIBMGR)
		$(CC) -o convert_icon convert_icon.o $(LIBMGR)

test2:		test2.o $(LIBMGR)
		$(CC) -o test2 test2.o $(LIBMGR)

chess:		chess.o $(LIBMGR)
		$(CC) -o chess chess.o $(LIBMGR)

mgrup:		mgrup.o $(LIBMGR)
		$(CC) -o mgrup mgrup.o $(LIBMGR)

cut:		cut.o $(LIBMGR)
		$(CC) -o cut cut.o $(LIBMGR)

unsqueeze:		unsqueeze.o
		$(CC) -o unsqueeze unsqueeze.o

squeeze:		squeeze.o
		$(CC) -o squeeze squeeze.o

pgs:		pgs.o $(LIBMGR)
		$(CC) -o pgs pgs.o $(LIBMGR)

pilot:		pilot.o $(LIBMGR)
		$(CC) -o pilot pilot.o $(LIBMGR)

pspreview:		pspreview.o $(LIBMGR)
		$(CC) -o pspreview pspreview.o $(LIBMGR)

browse:		browse.o $(LIBMGR)
		$(CC) -o browse browse.o $(LIBMGR)

walk:		walk.o $(LIBMGR)
		$(CC) -o walk walk.o $(LIBMGR)

iconmail.o:	mail_icons.h

iconmail:	iconmail.o $(LIBMGR)
		$(CC) -o iconmail iconmail.o $(LIBMGR)

iconmsgs.o:	msgs_icons.h

iconmsgs:	iconmsgs.o $(LIBMGR)
		$(CC) -o iconmsgs iconmsgs.o $(LIBMGR)

icontoc:	icontoc.o $(LIBMGR) $(BITLIB)
		$(CC) -o icontoc icontoc.o $(LIBMGR) $(BITLIB)

snap:		snap.o $(LIBMGR) $(BITLIB)
		$(CC) -o snap snap.o $(LIBMGR) $(BITLIB)

overlay:		overlay.o $(LIBMGR) $(BITLIB)
		$(CC) -o overlay overlay.o $(LIBMGR) $(BITLIB)

gt_scr:		gt_scr.o $(LIBMGR) $(BITLIB)
		$(CC) -o gt_scr gt_scr.o $(LIBMGR) $(BITLIB)

zoom:		zoom.o $(LIBMGR)  $(BITLIB)
		$(CC) -o zoom zoom.o $(LIBMGR) $(BITLIB)

mclock:		mclock.o $(LIBMGR) 
		$(CC) -o mclock mclock.o $(LIBMGR)

cycle:		cycle.o  $(LIBMGR)
		$(CC) -o cycle cycle.o $(LIBMGR)

window_print:	window_print.o  $(LIBMGR)
		$(CC) -o window_print window_print.o $(LIBMGR)

invert_colormap:	invert_colormap.o
		$(CC) -o invert_colormap invert_colormap.o -lpixrect

set_colormap:	set_colormap.o
		$(CC) -o set_colormap set_colormap.o -lpixrect

tj3:		tj3.o  $(LIBMGR)
		$(CC) -o tj3 tj3.o $(LIBMGR)

tj2:		tj2.o  $(LIBMGR)
		$(CC) -o tj2 tj2.o $(LIBMGR)

tj:		tj.o  $(LIBMGR)
		$(CC) -o tj tj.o $(LIBMGR)

flip:		flip.o 
		$(CC) -o flip flip.o

rotate:		rotate.o 
		$(CC) -o rotate rotate.o

tjfilter:	tjfilter.o 
		$(CC) -o tjfilter tjfilter.o

get_header:	get_header.o 
		$(CC) -o get_header get_header.o

cpp_filter:	cpp_filter.o
		$(CC) -o cpp_filter cpp_filter.o

# just in case 

$(LIBMGR):
		cd $(LIB); $(MAKE) FLAG=$(FLAG) libmgr.a

$(BITDIR)/blitlib.a:
		cd $(BITDIR); $(MAKE) FLAG=$(FLAG) blitlib.a

$(ALL):		$(INCL)/term.h

install:	$(ALL) $(INSDIR) $(INSDIR)/old
		-cd $(INSDIR);  mv -f $(ALL) old
		cp $(ALL) $(INSDIR)

$(INSDIR) $(INSDIR)/old:
		mkdir $@

clean:	
		rm -f *.o core

clobber:
		rm -f $(ALL)

list:
	@for i in ${CFILES} ${HFILES} ${OTHER}; do \
		echo "${START}/$$i"; \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
