#		makefile
ALL = se

MGRLIB=
FLAG=-g
CFLAGS= -DXMENU $(FLAG) -I/usr/mgr/include
INSDIR=../../bin
CFILES=se.c se_subs.c se_menus.c se_icon.c se.h
OTHER=Makefile README

all:		$(ALL)

se:	se.o ../b2i.o se_subs.o se_menus.o
			$(CC) $(CFLAGS) -o se se.o se_subs.o se_menus.o se_icon.o ../b2i.o \
			  -lmgr

../b2i.o:	
		cd ..;make b2i.o

se.o se_subs.o se_menus.o:	se.h

clean:
			-rm -f *.o core foo* 

clobber:
			-rm -f $(ALL)

install:
	@cp $(ALL) $(INSDIR)

list:
	@for i in ${CFILES} ${OTHER}; do \
		echo "${START}/$$i";\
	done	 

