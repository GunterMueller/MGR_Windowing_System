
START=.
INSROOT=../../..
INSDIR = $(INSROOT)/bin
MGRLIB=../../../lib
BITLIB=../../../src/port
CPIO=vcr.cpio

CFLAGS= -I$(MGRLIB) -I$(BITLIB) $(FLAG)

SRCS= README Makefile buttons vcr.1 vcr.c buttons.c

ALL=vcr

all:		$(ALL)

vcr:		vcr.o buttons.o
		cc -o vcr vcr.o buttons.o $(MGRLIB)/libmgr.a

#
#buttons.c:	buttons
#		icontoc buttons button_icon > buttons.c

clean:
		rm -f *.o core foo*

clobber:
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

cpio:
	make -s list | cpio -ocv > ${CPIO}

#	in case its not already made

$(MGRLIB)/libmgr.a:
	cd $(MGRLIB); make libmgr.a
