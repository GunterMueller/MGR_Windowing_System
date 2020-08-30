#		Makefile for fades

START=.
CFLAGS = -I$(BIT) $(FLAG)
CC=cc
INSROOT=../../..
MGR=../../..
LIB=$(MGR)/lib
BIT=$(MGR)/src/port
INSDIR=../../../bin
SRCS= fade.c fade.1 Makefile README

ALL=fade

all:	$(ALL)

fade:	fade.o $(BIT)/blitlib.a $(LIB)/libmgr.a
			$(CC) $(LFLAGS) -o fade fade.o $(BIT)/blitlib.a $(LIB)/libmgr.a

clean:
		rm -f *.o core foo*

clobber:	clean
		rm -f $(ALL)

install: $(ALL) $(INSDIR) $(INSDIR)/old
	-cd $(INSDIR); mv -f $(ALL) old
	-cp $(ALL) $(INSDIR)

$(INSDIR) $(INSDIR)/old:
		mkdir $@

list:
	@for i in ${SRCS}; do \
		echo "${START}/$$i";\
	done	

#	these are normally made first, but just in case:

$(LIB)/libmgr.a:
					cd $(LIB); make libmgr.a

$(BIT)/blitlib.a:
					cd $(BIT); make PASS=-DMOVIE FLAG=-O blitlib.a
