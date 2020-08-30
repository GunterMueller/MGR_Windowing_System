#	  Makefile for play_data

FLAG=-g
START=.
MGR=../../..
SRC=$(MGR)/src
SHARE=$(SRC)/port
LIB=$(MGR)/lib
CFLAGS= $(FLAG) -I$(SHARE) -DMOVIE
INSDIR=../../../bin
SRCS= \
	Makefile get_maps.1 get_maps.c play_data.1 play_data.c \
	shrink.1 shrink.c shrink_map.1 shrink_map.c time_it.1 \
	time_it.sh to_ascii.c to_ascii.1 to_binary.1 to_binary.c

ALL = 	play_data to_ascii to_binary get_maps shrink shrink_map time_it

all:	$(ALL)

play_data:		play_data.o
					$(CC) $(CFLAGS) -o play_data play_data.o $(SHARE)/blitlib.a

to_ascii:		to_ascii.o
					$(CC) $(CFLAGS) -o to_ascii to_ascii.o

shrink:			shrink.o shrink_map.o
					$(CC) $(CFLAGS) -o shrink shrink.o shrink_map.o \
								$(SHARE)/blitlib.a

shrink_map:		shrink_map.c
					$(CC) $(CFLAGS) -DTEST -o shrink_map shrink_map.c \
								$(LIB)/libmgr.a $(SHARE)/blitlib.a

get_maps:		get_maps.o
					$(CC) $(CFLAGS) -o get_maps get_maps.o \
							$(LIB)/libmgr.a $(SHARE)/blitlib.a

to_binary:		to_binary.o
					$(CC) $(CFLAGS) -o to_binary to_binary.o

play_data.o to_ascii.o to_binary.o:		$(SHARE)/share.h

time_it:			time_it.sh
					cp time_it.sh time_it
					chmod +x time_it
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

#	these are normally made first, but just in case:

$(LIB)/libmgr.a:
					cd $(LIB); make libmgr.a

$(SHARE)/blitlib.a:
					cd $(SHARE); make PASS=-DMOVIE FLAG=-O blitlib.a
