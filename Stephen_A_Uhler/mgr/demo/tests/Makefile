#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.1 88/06/21 14:02:01 bianchi Exp $
#	$Source: /tmp/mgrsrc/demo/tests/RCS/Makefile,v $

# makefile for demo test routines

LIB= ../../lib/libmgr.a
INCL= -I../../lib
CPIO=test.cpio
CFLAGS= $(INCL) $(FLAG)
START=.

CFILES= test_rop.c test_text.c test_menu.c half.c many.c

ALL =	test_rop test_text test_menu half many send

all small:	$(ALL)

fast:		$(CFILES)
		cc -c $(CFLAGS) $(CFILES)
		make all

test_menu:	test_menu.o $(LIB)
		cc $(CFLAGS) -o test_menu test_menu.o $(LIB)

scale:	scale.o $(LIB)
		cc $(CFLAGS) -o scale scale.o $(LIB)

send:	send.o $(LIB)
		cc $(CFLAGS) -o send send.o $(LIB)

blank:	blank.o $(LIB)
		cc $(CFLAGS) -o blank blank.o $(LIB)

colors:	colors.o $(LIB)
		cc $(CFLAGS) -o colors colors.o $(LIB)

colors2:	colors2.o $(LIB)
		cc $(CFLAGS) -o colors2 colors2.o $(LIB)

spline:	spline.o $(LIB)
		cc $(CFLAGS) -o spline spline.o $(LIB)

cut:	cut.o $(LIB)
		cc $(CFLAGS) -o cut cut.o $(LIB)

circles:	circles.o $(LIB)
		cc $(CFLAGS) -o circles circles.o $(LIB)

lines:	lines.o $(LIB)
		cc $(CFLAGS) -o lines lines.o $(LIB)

snap:	snap.o $(LIB)
		cc $(CFLAGS) -o snap snap.o $(LIB)

many:	many.o $(LIB)
		cc $(CFLAGS) -o many many.o $(LIB)

cal:		cal.o $(LIB)
		cc $(CFLAGS) -o cal cal.o $(LIB)

test_icon:		test_icon.o $(LIB)
		cc $(CFLAGS) -o test_icon test_icon.o $(LIB)

half:		half.o $(LIB)
		cc $(CFLAGS) -o half half.o $(LIB)

test_rop:	test_rop.o $(LIB)
		cc $(CFLAGS) -o test_rop test_rop.o $(LIB)

test_text:	test_text.o $(LIB)
		cc $(CFLAGS) -o test_text test_text.o $(LIB)

clean:
		rm -f *.o

clobber:
		rm -f $(ALL)

install:

list:
	@for i in $(CFILES) Makefile; do \
		echo "$(START)/$$i"; \
	done

cpio:
	make -s list | cpio -ocv > $(CPIO)
