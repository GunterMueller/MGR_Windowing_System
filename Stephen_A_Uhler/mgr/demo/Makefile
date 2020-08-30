#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.6 88/08/22 10:20:28 sau Exp $
#	$Source: /tmp/mgrsrc/demo/RCS/Makefile,v $

SUBDIR= icon misc movie plot sh ify tests 
INSROOT=/usr/mgr
INSDIR = $(INSROOT)/bin

# libraries

MGR=..
LIB = $(MGR)/lib
LIBMGR = $(LIB)/libmgr.a
BITDIR=$(MGR)/src/port

START=.
MAKE=make
CPIO=demo.cpio

all: libs
	for i in ${SUBDIR}; do \
	(cd $$i; echo "	making demo/$$i";\
 	 $(MAKE)  FLAG=${FLAG} INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

install:
	for i in ${SUBDIR}; do \
	(cd $$i; echo " $@ing demo/$$i";\
 	 $(MAKE)  FLAG=${FLAG} INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

#	librarys the demos may need 

libs: $(LIBMGR) $(BITDIR)

$(LIBMGR):
		cd $(LIB); $(MAKE) FLAG=$(FLAG) libmgr.a

$(BITDIR)/blitlib.a:
		cd $(BITDIR); $(MAKE) FLAG=$(FLAG) blitlib.a

#	cleanup

clean clobber:
	-for i in ${SUBDIR}; do \
	(cd $$i; echo "	$@ing demo/$$i";\
 	 $(MAKE)  INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

list:
	@echo ${START}/Makefile
	@echo ${START}/README
	@for i in ${SUBDIR}; do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		(cd $$i; $(MAKE) START=${START}/$$i list; cd ..);\
		fi \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
