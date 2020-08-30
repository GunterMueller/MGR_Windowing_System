#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.4 88/07/19 14:34:19 sau Exp $
#	$Source: /tmp/mgrsrc/src/oblit/RCS/Makefile,v $

#	makefile for stub blit library

CFLAGS=-O  -I..

CFILES= blit.c line.c pixel.c bitmap.c
OBJS= blit.o line.o pixel.o bitmap.o
HFILES= bitmap.h
OTHER=Makefile README
LIB=blitlib.a
START=.
MAKE=make

all:		$(LIB)

# bitmap library

$(LIB): $(OBJS)
	ar rv $(LIB) $(OBJS)
	ranlib $(LIB)

clean:
	rm -f *.o $(LIB)

list:
	@for i in ${HFILES} ${CFILES} ${OTHER}; do \
		echo "${START}/$$i"; \
	done	

clobber:
