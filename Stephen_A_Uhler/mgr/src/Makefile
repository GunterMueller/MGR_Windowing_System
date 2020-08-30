:                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: /home/sau/mgr/src/RCS/Makefile,v 1.3 91/03/14 11:15:08 sau Exp Locker: sau $
#	$Source: /home/sau/mgr/src/RCS/Makefile,v $

#
#	compile flags:
#	  WHO:		muck utmp file so "who" works
#	  VI		code for vi mouse hack
#	  DEBUG		turns on lots of debugging code (for -d option)
#	  FASTMOUSE	xor mouse track
#	  BUCKEY	for keyboard operated "mouse" commands
#	  PRIORITY	for priority window scheduling instead of 
#			round-robin; the active window gets higher priority
#	  CUT		for cut/paste (don't use)
#	  ALIGN		forces window alignment for fast scrolling
#			use with 68010/68020 version only
#	  ALIGN32	Obsolete (expunged)
#	  WEIRD		does weird thing on window reshapes
#	  KILL		kills windows upon tty i/o errors
#
#	  MALLOC	for malloc debugging code - you supply the malloc
#
#	  SHRINK	use only some of the screen (use MGRSIZE in the environment)
#
#	  NOSTACK	don't permit event stacking 
#
#	  BELL		really ring the bell (works on at least sun 3's and 4`s)
#			You need to do a "mknod /dev/bell c 12 2" (for sun3)
#			You need to do a "ln -s /dev/kbd /dev/bell" (for sun4)
#
#	  KBD		read mgr input from the sun kbd, instead of stdin
#			This permits redirection of console msgs to a window
#
#	  FRACCHAR	fractional character movement stuff (experimental)
#
#	  XMENU		extended menu stuff (experimental)
#
#	  MOVIE		for movie making extensions
#
#####################################################################
#	bitblit libraries
#	   blit - template based 68020 asm code (fastest)
#					uses 32 bit alignment
#	 	oblit - crufty sun-2 specific code (obsolete, use port instead)
#					uses 16 bit alignment
#     port - template based portable "C" code  (monochrome only for now)
#					uses 32 bit alignment (or 16 or 8)
#					use -DMOVIE to turn on logging
#	   pixrect - SUN's pixrect library: Useful for color
#					uses 16 bit alignment (mostly)
#		stub_lib - Stub libraries for use as a basis for porting

LIBS=port blit oblit pixrect stub_lib

#
#	which bit-blit library to use

BLITDIR=port
BIT_LIB =   $(BLITDIR)/blitlib.a

#	Use fast scrolling code (only works on 68010/68020 with BLITDIR=blit)

#DDEP = -DALIGN
#MACHDEP_C = fast_scroll.c

#	Specify other libraries here (use -lpixrect for BLITDIR=pixrect)

OTHERLIB =
INCL = ../lib

#	specific stuff goes here

MACHDEP_H =
MACHDEP_C =
MACHDEP_O =

#	This version number is unrelated to anything

VER = 3.3.3
#
# built-in path names:
#
###########################################

# root of MGR to find fonts and icons - only builtin pathname
INSROOT=/usr/mgr

# optimization flag - usually passed in from above
#FLAG = -O
#FLAG = -g

###########################################

START=.
DSAME = -DWHO -DVI -DBUCKEY -DKILL -DCUT -DFRACCHAR -DXMENU
#	FONTDIR:	where to find font files  (default: /usr/mgr/font)
#	ICONDIR:	where to find icons	  (default: /usr/mgr/icon)
PATHFLAGS = -DFONTDIR=\"$(INSROOT)/font\" -DICONDIR=\"$(INSROOT)/icon\"
#
DFLAGS = $(DSAME) $(DDEP)
CFLAGS = $(FLAG) $(DFLAGS) $(PATHFLAGS) -I. -I$(BLITDIR) -I$(INCL)
FONT_LIB = ../font
DEFAULT_FONT = $(FONT_LIB)/default.fnt
INSDIR = $(INSROOT)/bin
CPIO=src.cpio
TAR=src.tar
DEPEND = make.depend
LOG=make.log
#
#	Versions of MGR
#		mgr			- the default
#		colormgr		- makes color mgr using SUN's pixrect library
#		pixmgr		- makes a monochrome MGR using pixrect
#		prodmgr		- makes an optimized (680x0) MGR with debugging stripped
#		oldmgr		- Makes a pre 3.4 release (no special kbd and bell handling)
#		sun2mgr		- makes a sun2 version (old bitblit code) [obsolete]
#		oldsun2mgr	- makes a sun2 (release 1.x, old bitblit code) [obsolete]
#		portmgr		- makes a monochrome MGR using the portable blit code
#							Use this one for a monochrome Sparc station
#		stubmgr		- make a monochrome MGR with NO bitblit code
#							For testing

PORTCFILES = \
	Write.c \
	bitmaphead.c \
	bitmapread.c \
	border.c \
	copyright.c \
	cut.c \
	data.c \
	debug_flags.c \
	destroy.c \
	do_buckey.c \
	do_button.c \
	do_event.c \
	do_menu.c \
	down_load.c \
	erase_win.c \
	font_subs.c \
	get_font.c \
	get_info.c \
	get_menus.c \
	get_rect.c \
	get_text.c \
	getshell.c \
	graph_subs.c \
	intersect.c \
	kbd.c \
	mgr.c \
	mouse_get.c \
	move.c \
	move_box.c \
	new_window.c \
	popen.c \
	print.c \
	put_window.c \
	scroll.c \
	set_mode.c \
	shape.c \
	sigdata.c \
	startup.c \
	subs.c \
	update.c \
	utmp.c \
	win_make.c \
	win_stack.c \
	win_subs.c \
	write_ok.c 

PORTOFILES = \
	Write.o \
	bitmaphead.o \
	bitmapread.o \
	border.o \
	copyright.o \
	cut.o \
	data.o \
	debug_flags.o \
	destroy.o \
	do_buckey.o \
	do_button.o \
	do_event.o \
	do_menu.o \
	down_load.o \
	erase_win.o \
	font_subs.o \
	get_font.o \
	get_info.o \
	get_menus.o \
	get_rect.o \
	get_text.o \
	getshell.o \
	graph_subs.o \
	intersect.o \
	kbd.o \
	mgr.o \
	mouse_get.o \
	move.o \
	move_box.o \
	new_window.o \
	popen.o \
	print.o \
	put_window.o \
	scroll.o \
	set_mode.o \
	shape.o \
	sigdata.o \
	startup.o \
	subs.o \
	update.o \
	utmp.o \
	win_make.o \
	win_stack.o \
	win_subs.o \
	write_ok.o 

CFILES = $(PORTCFILES) $(MACHDEP_C)

OFILES = $(PORTOFILES) $(MACHDEP_O)

#	these are for malloc debugging
#MALLOC=/usr/lib/debug/malloc.o /usr/lib/debug/mallocmap.o
#MFLAGS=-DMALLOC

HFILES = \
	$(INCL)/dump.h $(INCL)/window.h \
	clip.h default_font.h defines.h defs.h event.h \
	font.h icons.h menu.h \
	$(MACHDEP_H)

OTHER = \
	Makefile \
	copyright_data.c \
	compile_font.c dependencies fast_scroll.c

instructions:
	@echo "For all (modern) monochome versions:		make portmgr"
	@echo "For all color versions:				make colormgr"
	@echo "For FAST 680x0 (sun[23]) monochrome versions:	make prodmgr"
	@echo "For other configurations, see the Makefile"
	@echo "remember to set: FLAG=-g to include debugging info"
	@echo "remember to set: FLAG=-O to optimize"

############################################################################
all:	$(MGR) 
############################################################################

purify:
	purify -v cc -o pmgr $(PORTOFILES) copyright_data.o version.o port/blitlib.a

testpurify:
	cc -o pmgr $(PORTOFILES) copyright_data.o version.o port/blitlib.a

############################################################################
# stub mgr, using dummy bitblit code
stubmgr:
	cd stub_lib; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DDEBUG -DKILL" \
		INSROOT=$(INSROOT) \
		DDEP= \
		MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		OTHERLIB= \
		BLITDIR=stub_lib \
		FLAG=$(FLAG) \
		MGR=mgr_stub \
		VER=2.3 all

############################################################################
# port mgr, using portable bitblit code
# This will also make a sun-[234] monochrome version
portmgr:
	cd port; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="$(DSAME) -DDEBUG -DSHRINK -DBELL -DKBD -DMOVIE"  \
		INSROOT=$(INSROOT) \
		DDEP= \
		MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		OTHERLIB= \
		BLITDIR=port \
		FLAG="$(FLAG)" \
		PASS=-DMOVIE\
		MGR=mgr_port \
		VER=2.3 all

############################################################################
# port mgr, using portable bitblit code   EXPERIMENTAL NETWORK VERSION
# This will also make a sun-[234] monochrome version
Xportmgr:
	cd port; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="$(DSAME) -DDEBUG -DSHRINK -DMOVIE"  \
		INSROOT=$(INSROOT) \
		DDEP= \
		MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		OTHERLIB= \
		BLITDIR=port \
		FLAG="$(FLAG)" \
		PASS=-DMOVIE\
		MGR=mgr_port \
		VER=2.3 all

############################################################################
# port mgr, using portable bitblit code (OPTIMIZED VERSION)
# This will also make a sun-[234] monochrome version
Portmgr:
	cd port; $(MAKE) FLAG=-O4
	$(MAKE) DSAME="$(DSAME) -DSHRINK -DBELL -DKBD -DMOVIE"  \
		INSROOT=$(INSROOT) \
		DDEP= \
		MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		OTHERLIB= \
		BLITDIR=port \
		FLAG="-O4" \
		PASS=-DMOVIE\
		MGR=mgr_port \
		VER=2.3 all

############################################################################
# color mgr, for sun 3/60, sun3/110 and sun-roadrunner   - uses pixrect
colormgr:
	cd pixrect; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY  -DKILL -DKBD -DBELL \
			-DSHRINK -DCOLOR -DFASTMOUSE" \
		INSROOT=$(INSROOT) \
		DDEP= \
		MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		OTHERLIB=-lpixrect \
		BLITDIR=pixrect \
		FLAG="$(FLAG)" \
		MGR=mgr_color \
		VER=2.3 all

############################################################################
# sun 3's production release (no debugging code, optimized), uses 680x0 asm
prodmgr:
	cd blit; $(MAKE) FLAG=-O
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DKILL -DCUT -DALIGN \
			-DBELL -DKBD -DFRACCHAR"  \
		INSROOT=$(INSROOT) \
		MACHDEP_C=fast_scroll.c \
		MACHDEP_O=fast_scroll.o \
		FLAG=-O \
		BLITDIR=blit \
		MGR=mgr_prod all

############################################################################
# sun 3's (68020 based) pre release 3.4
oldmgr:
	cd blit; $(MAKE) FLAG=-O
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DDEBUG -DKILL -DCUT -DALIGN"  \
		INSROOT=$(INSROOT) \
		FLAG="$(FLAG)" \
		MACHDEP_C = fast_scroll.c \
		BLITDIR=blit \
		MACHDEP_O = fast_scroll.o \
		MGR=mgr_old all

############################################################################
# sun 2's (68010 based)
sun2mgr:
	cd oblit; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DDEBUG -DKILL -DCUT -DBELL -DKBD \
		       -DALIGN" \
		INSROOT=$(INSROOT) \
		FLAG="$(FLAG)" \
		MACHDEP_C = fast_scroll.c \
		MACHDEP_O = fast_scroll.o \
		BLITDIR=oblit \
		MGR=mgr_sun2 all

############################################################################
# sun 2's (68010 based) running pre-3.4 releases (i.e. 1.0)
oldsun2mgr:
	cd oblit; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DDEBUG -DKILL -DCUT -DALIGN" \
		INSROOT=$(INSROOT) \
		FLAG="$(FLAG)" \
		BLITDIR=oblit \
		MACHDEP_C = fast_scroll.c \
		MACHDEP_O = fast_scroll.o \
		MGR=mgr_oldsun2 all

############################################################################
pixmgr:
	cd pixrect; $(MAKE) FLAG=$(FLAG)
	$(MAKE) DSAME="-DWHO -DVI -DBUCKEY -DDEBUG -DKILL -DCUT -DBELL -DKBD" \
		INSROOT=$(INSROOT) \
		DDEP= MACHDEP_C= MACHDEP_H= MACHDEP_O= \
		BLITDIR=pixrect OTHERLIB=-lpixrect \
		FLAG="$(FLAG)" \
		MGR=mgr_pix \
		VER=2.3 all

########################################################################

$(MGR):	$(BIT_LIB) $(OFILES) \
			copyright_data.o version.o 
	-rm -f _$(MGR)
	-mv $(MGR) _$(MGR)
	$(CC) $(LFLAGS) $(FLAG) -o $(MGR) $(MALLOC) $(OFILES) \
		version.o copyright_data.o $(BIT_LIB) $(OTHERLIB)
	@date >> $(LOG)
	echo "changed $?" >> $(LOG)
	@size $(MGR)
	-ln $(MGR) mgr

default_font.h:
		make compile_font
		./compile_font < $(DEFAULT_FONT) default_font \
			     > default_font.h

compile_font:	compile_font.o
		$(CC) $(LFLAGS) -o compile_font compile_font.o

#	create mgr version information

version.c:	$(OFILES)
		 echo "/* compilation info for $(MGR) */"	 > version.c
		@echo " "					>> version.c
		@echo "char *version[]={"			>> version.c
		@echo "   \"$(VER)\",\"`hostname`\",\"`date`\",">> version.c
		@echo "   \"`echo $(DFLAGS) | sed 's/-D//g'`\",">> version.c
		@echo "   \"$(INSROOT)\","			>> version.c
		@echo "   \"`whoami`\","			>> version.c
		@echo "   \"`strings /vmunix | grep 'SunOS'`\",">> version.c
		@echo "   \"$(FLAG)\","			>> version.c
		@echo "   \"$(BLITDIR)\","			>> version.c
		@echo "   };"					>> version.c 

# The optimizer botches on this one

fast_scroll.o:	fast_scroll.c
		$(CC) -c -I$(BLITDIR) fast_scroll.c

#	put copyright data into text segment

copyright_data.o:	copyright_data.c
		$(CC) -c -R copyright_data.c

#	make the bitblit library

$(BLITDIR)/blitlib.a:
		cd $(BLITDIR); $(MAKE) PASS=$(PASS) FLAG=$(FLAG) blitlib.a

$(LOG):		
		echo "MGR version log > $(LOG)

clean:		
		rm -f *.o core version.c
		@for i in $(LIBS); do \
			(echo "cleaning $$i"; cd $$i; $(MAKE) clean); \
		done	

clobber:
		rm -f compile_font mgr mgr_*
		@for i in $(LIBS); do \
			(echo "clobbering $$i"; cd $$i; $(MAKE) clobber); \
		done	

install:	mgr $(INSROOT) $(INSDIR) /dev/bwtwo0 /dev/bell
		-cd $(INSDIR);  mv -f mgr mgr.old
		-cp mgr $(INSDIR)
		-/etc/chown root $(INSDIR)/mgr  && \
			 chmod u+s,go-w $(INSDIR)/mgr

mgr:
		$(MAKE) INSROOT=$(INSROOT) FLAG=$(FLAG) $(TARGET)

$(INSROOT) $(INSDIR):
		mkdir $@

/dev/bwtwo0:
		@echo "making /dev/bwtwo0"
		-/bin/su root -c 'cd /dev;  MAKEDEV bwtwo0'

/dev/bell:
		@echo '*** Don't forget to make /dev/bell if you want MGR to beep ***'

tar:		
		tar cf $(TAR) `make list`
		
depend:	
		dependencies `$(MAKE) list` > $(DEPEND)
		echo " header file dependencies written to $(DEPEND)"

list:
	@for i in $(HFILES) $(CFILES) $(OTHER); do \
		echo "$(START)/$$i"; \
	done	
	@for i in $(LIBS); do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		cd $$i; $(MAKE) START=${START}/$$i list; cd ..;\
		fi \
	done	

cpio:
	make -s list  |  sort -u  |  cpio -ocv > $(CPIO)

# header file dependencies go here  (generated by make depend)

Write.o: \
	Write.c

bitmaphead.o: \
	bitmaphead.c $(INCL)/dump.h $(BLITDIR)/bitmap.h

bitmapread.o: \
	bitmapread.c $(INCL)/dump.h $(BLITDIR)/bitmap.h

border.o: \
	border.c $(BLITDIR)/bitmap.h defs.h defines.h

copyright.o: \
	copyright.c $(BLITDIR)/bitmap.h

cut.o: \
	cut.c $(BLITDIR)/bitmap.h defs.h defines.h font.h $(INCL)/window.h \
	event.h

data.o: \
	data.c $(BLITDIR)/bitmap.h defs.h defines.h font.h icons.h

debug_flags.o: \
	debug_flags.c

destroy.o: \
	destroy.c $(BLITDIR)/bitmap.h defs.h defines.h font.h event.h

do_buckey.o: \
	do_buckey.c $(BLITDIR)/bitmap.h defs.h defines.h font.h \
	event.h

do_button.o: \
	do_button.c $(BLITDIR)/bitmap.h menu.h defs.h defines.h \
	font.h event.h

do_event.o: \
	do_event.c $(BLITDIR)/bitmap.h defs.h defines.h font.h \
	event.h

do_menu.o: \
	do_menu.c $(BLITDIR)/bitmap.h menu.h defs.h defines.h font.h

down_load.o: \
	down_load.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	menu.h event.h $(INCL)/window.h

erase_win.o: \
	erase_win.c $(BLITDIR)/bitmap.h defs.h defines.h

font_subs.o: \
	font_subs.c $(BLITDIR)/bitmap.h font.h default_font.h $(INCL)/window.h

get_font.o: \
	get_font.c $(BLITDIR)/bitmap.h font.h

get_info.o: \
	get_info.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	menu.h event.h $(INCL)/window.h

get_menus.o: \
	get_menus.c $(BLITDIR)/bitmap.h menu.h font.h

get_rect.o: \
	get_rect.c $(BLITDIR)/bitmap.h defs.h defines.h

get_text.o: \
	get_text.c $(BLITDIR)/bitmap.h defs.h defines.h font.h \
	event.h

getshell.o: \
	getshell.c $(BLITDIR)/bitmap.h defs.h defines.h

graph_subs.o: \
	graph_subs.c $(BLITDIR)/bitmap.h

intersect.o: \
	intersect.c $(BLITDIR)/bitmap.h defs.h defines.h

kbd.o: \
	kbd.c $(BLITDIR)/bitmap.h defs.h defines.h

mgr.o: \
	mgr.c $(BLITDIR)/bitmap.h defs.h defines.h menu.h font.h \
	event.h

mouse_get.o: \
	mouse_get.c

move.o: \
	move.c $(BLITDIR)/bitmap.h defs.h defines.h event.h

move_box.o: \
	move_box.c $(BLITDIR)/bitmap.h defs.h defines.h

new_window.o: \
	new_window.c $(BLITDIR)/bitmap.h defs.h defines.h $(INCL)/window.h \
	font.h menu.h

print.o: \
	print.c

put_window.o: \
	put_window.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	menu.h event.h $(INCL)/window.h clip.h

scroll.o: \
	scroll.c $(BLITDIR)/bitmap.h defs.h defines.h clip.h

set_mode.o: \
	set_mode.c

shape.o: \
	shape.c $(BLITDIR)/bitmap.h defs.h defines.h font.h event.h

sigdata.o: \
	sigdata.c

startup.o: \
	startup.c $(BLITDIR)/bitmap.h defs.h defines.h font.h

subs.o: \
	subs.c $(BLITDIR)/bitmap.h font.h defs.h defines.h event.h \
	$(INCL)/window.h

update.o: \
	update.c $(BLITDIR)/bitmap.h defs.h defines.h clip.h

utmp.o: \
	utmp.c

win_make.o: \
	win_make.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	$(INCL)/window.h event.h

win_stack.o: \
	win_stack.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	$(INCL)/window.h menu.h event.h

win_subs.o: \
	win_subs.c $(BLITDIR)/bitmap.h font.h defs.h defines.h \
	$(INCL)/window.h clip.h

write_ok.o: \
	write_ok.c defines.h

fast_scroll.o: \
	fast_scroll.c $(BLITDIR)/bitmap.h

copyright_data.o: \
	copyright_data.c

compile_font.o: \
	compile_font.c $(BLITDIR)/bitmap.h font.h

