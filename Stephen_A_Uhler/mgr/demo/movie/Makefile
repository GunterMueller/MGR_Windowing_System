#                        Copyright (c) 1991 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.6 88/08/22 10:20:28 sau Exp $
#	$Source: /tmp/mgrsrc/demo/RCS/Makefile,v $

SUBDIR= fade play_data roll vcr audio 
INSROOT=/usr/mgr
INSDIR = $(INSROOT)/bin
START=.
MAKE=make
CPIO=demo.cpio
SCRIPTS = make_movie play_movie

all small fast:	$(SCRIPTS)
	for i in ${SUBDIR}; do \
	(cd $$i; echo "	making demo/$$i";\
 	 $(MAKE)  FLAG=${FLAG} INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

make_movie:
	< make_movie.sh sed "s@XXXX@$(INSDIR)@" > make_movie
	chmod +x make_movie

play_movie:
	< play_movie.sh sed "s@XXXX@$(INSDIR)@" > play_movie
	chmod +x play_movie

install: $(SCRIPTS) $(INSDIR) $(INSDIR)/old
	-cd $(INSDIR); mv -f $(SCRIPTS) old
	-cp $(SCRIPTS) $(INSDIR)
	for i in ${SUBDIR}; do \
	(cd $$i; echo "	$@ing demo/$$i";\
 	 $(MAKE)  FLAG=$(FLAG) INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

$(INSDIR) $(INSDIR)/old:
		mkdir $@

clean:
	-for i in ${SUBDIR}; do \
	(cd $$i; echo "	$@ing demo/$$i";\
 	 $(MAKE)  INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

clobber:
	-rm -f  $(SCRIPTS)
	-for i in ${SUBDIR}; do \
	(cd $$i; echo "	$@ing demo/$$i";\
 	 $(MAKE)  INSROOT=${INSROOT} INSDIR=${INSDIR} $@;\
	 cd ..);\
	done	

list:
	@echo ${START}/Makefile
	@echo ${START}/README
	@echo ${START}/make_movie.sh
	@echo ${START}/play_movie.sh
	@for i in ${SUBDIR} demo; do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		(cd $$i; $(MAKE) START=${START}/$$i list; cd ..);\
		fi \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
