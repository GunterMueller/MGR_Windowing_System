#		Makefile for roll: credit generator

CFLAGS = -g -I$(BIT) -DFONT_DIR=\"$(FONTDIR)\"
START=.
INSROOT=../../..
MGR=../../..
LIB=$(MGR)/lib
BIT=$(MGR)/src/port
FONTDIR=`pwd`/fonts
INSDIR=$(INSROOT)/bin
CPIO=roll.cpio

SRCS = \
	README Makefile assemble.c restore.c roll.1 roll.c save.c trim.c
SUBDIR=fonts scripts

ALL=roll

all:	$(ALL)

assemble:	assemble.o
			$(CC) $(CFLAGS) -o assemble assemble.o -lmgr -lblit

trim:	trim.o
			$(CC) $(CFLAGS) -o trim trim.o -lmgr -lblit

restore:	restore.o $(BIT)/blitlib.a $(LIB)/libmgr.a
			$(CC) $(CFLAGS) -o restore restore.o $(BIT)/blitlib.a $(LIB)/libmgr.a

save:	save.o $(BIT)/blitlib.a $(LIB)/libmgr.a
			$(CC) $(CFLAGS) -o save save.o $(BIT)/blitlib.a $(LIB)/libmgr.a

color:
			make croll BIT=$(HOME)/mgr/nsrc/port8 

croll:	croll.o $(BIT)/blitlib.a $(LIB)/libmgr.a
			$(CC) $(CFLAGS) -o croll croll.o $(BIT)/blitlib.a $(LIB)/libmgr.a

roll:	roll.o $(BIT)/blitlib.a $(LIB)/libmgr.a
			$(CC) $(CFLAGS) -o roll roll.o $(BIT)/blitlib.a $(LIB)/libmgr.a

#		these are normally made first, but just in case:

$(LIB)/libmgr.a:
			(cd $(LIB); $(MAKE) libmgr.a)
	
$(BIT)/blitlib.a:
			(cd $(BIT); $(MAKE) blitlib.a)
	
clean:
		rm -f *.o core foo*

clobber:	clean
		rm -f $(ALL)

install: $(ALL) $(INSDIR) $(INSDIR)/old
	-cd $(INSDIR); mv -f $(ALL) old
	cp $(ALL) $(INSDIR)

$(INSDIR) $(INSDIR)/old:
		mkdir $@

list:
	@for i in ${SRCS}; do \
		echo "${START}/$$i";\
	done	 
	@for i in ${SUBDIR}; do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		(cd $$i; $(MAKE) START=${START}/$$i list; cd ..);\
		fi \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}

