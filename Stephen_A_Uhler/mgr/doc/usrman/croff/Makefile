#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 1.4 88/07/08 13:51:00 sau Exp $
#	$Source: /tmp/mgrsrc/doc/usrman/croff/RCS/Makefile,v $

# Makefile for Troff C-Preprocessor

START=.

#	hash table size (for keywords)
BUCKETS = 59
CFLAGS = -g -DHSIZE=$(BUCKETS)
CPIO=croff.cpio

croff:		keys.o croff.o hash.o
		cc -o croff croff.o hash.o keys.o

keys.c:		keys.src mktable
		< keys.src ./mktable -n keywords $(BUCKETS) > keys.c

mktable:	mktable.o hash.o
		cc $(CFLAGS) -o mktable mktable.o hash.o

clean:		
		rm -f *.o core keys.c mktable lex.yy.c

clobber:		clean
		rm -f croff

list:
	@for i in Makefile README croff.1 croff.l hash.c hash.h keys.src mktable.c; do \
		echo "$(START)/$$i"; \
	done

cpio:
		make -s list | cpio -ocva > $(CPIO)

hash.o mktable.o:	hash.h

install: croff
