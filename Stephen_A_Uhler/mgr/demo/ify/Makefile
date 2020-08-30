# Makefile for ify

CC = cc

# change these lines to say where to find the mgr #include files and libraries
# perhaps add -DOLDMGR to the end of CFLAGS

START=.
INSROOT=/usr/mgr
INSDIR=$(INSROOT)/bin
CFLAGS = -I../../lib $(FLAG)
MGRLIB = ../../lib/libmgr.a

SRCS=Makefile icondata.c ify.1 ify.c setname.1 setname.c

ALL = ify setname
all: $(ALL)

ify: ify.o icondata.o
	$(CC) $(CFLAGS) ify.o icondata.o $(MGRLIB) -o ify

ify.o: ify.c

icondata.o: icondata.c

setname: setname.c
	$(CC) $(CFLAGS) setname.c $(MGRLIB) -o setname

install:	$(ALL) $(INSDIR)
		cd $(INSDIR);  rm -f $(ALL)
		cp $(ALL) $(INSDIR)

#		man pages are installed separately now
#		-mkdir $(INSROOT)/man $(INSROOT)/man/man1
#		cp setname.1 ify.1 $(INSROOT)/man/man1

clean:
	rm  -f core *.o

clobber:
	rm -f $(ALL)

list:
	@for i in ${SRCS}; do \
		echo "${START}/$$i";\
	done	

$(INSDIR):
		mkdir $@
