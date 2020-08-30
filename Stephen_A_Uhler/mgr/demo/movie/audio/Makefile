#		makefile
ALL = \
		audiocntl audiowatch extract mac mac2 mix record \
		show_sound show_sounds play_audio

START=.
INSROOT=../../..
MGR=../../..
LIB=$(MGR)/lib
FLAG=-g
CFLAGS= $(FLAG) -I$(LIB)
INSDIR=$(INSROOT)/bin
SUBDIR=SE

CFILES= \
	audiocntl.c audiowatch.c b2i.c \
	extract.c mac.c mac2.c mix.c \
	record.c show_sound.c show_sounds.c \
	srecord.c play_audio.c

OTHER= Makefile README audiocntl.1 make_movie.1 play_audio.1 movie.1 record.1

all:		$(ALL) se

audiocntl:		audiocntl.o
			$(CC) $(CFLAGS) -o audiocntl audiocntl.o

mix:		mix.o  b2i.o
			$(CC) $(CFLAGS) -o mix mix.o b2i.o

mac2:	mac2.o
			$(CC) $(CFLAGS) -o mac2 mac2.o $(LIB)/libmgr.a

mac:	mac.o
			$(CC) $(CFLAGS) -o mac mac.o $(LIB)/libmgr.a

show_sounds:	show_sounds.o b2i.o
			$(CC) $(CFLAGS) -o show_sounds show_sounds.o b2i.o $(LIB)/libmgr.a

show_sound:	show_sound.o b2i.o
			$(CC) $(CFLAGS) -o show_sound show_sound.o b2i.o $(LIB)/libmgr.a

play_audio:	play_audio.o
			$(CC) $(CFLAGS) -o play_audio play_audio.o

srecord:	srecord.o
			$(CC) $(CFLAGS) -o srecord srecord.o

record:	record.o
			$(CC) $(CFLAGS) -o record record.o

extract:	extract.o
			$(CC) $(CFLAGS) -o extract extract.o

se:		
			cd SE; $(MAKE) se;

clean:
			-rm -f *.o core foo*
			cd SE; $(MAKE) clean

clobber:
			-rm -f $(ALL)
			cd SE; $(MAKE) clobber

install: $(ALL) $(INSDIR) $(INSDIR)/old
	-cd $(INSDIR); mv -f $(ALL) old
	cp $(ALL) $(INSDIR)
	cd SE; $(MAKE) INSDIR=$(INSDIR) install


$(INSDIR) $(INSDIR)/old:
		mkdir $@

list:
	@for i in ${CFILES} ${OTHER}; do \
		echo "${START}/$$i";\
	done	 
	@for i in ${SUBDIR}; do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		(cd $$i; $(MAKE) START=${START}/$$i list; cd ..);\
		fi \
	done	

#	in case its not already made

$(LIB)/libmgr.a:
	cd $(LIB); make libmgr.a
