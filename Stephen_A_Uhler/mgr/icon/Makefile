#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.7 88/08/17 09:01:30 sau Exp $
#	$Source: /tmp/mgrsrc/icon/RCS/Makefile,v $

#	icons for mgr
INSROOT = .
INSDIR=$(INSROOT)/icon
START=.
SUBDIRS=eye grey mouse logo
CPIO=icons.cpio
ICONS= \
	apple arrdown arrleft arrne arrright \
	arrup bandaid_bad bignot biplane blank \
	block blockmask bomb buttons cherries \
	cloudy cntl copter copy cpath \
	cube1 cube2 cube3 cube4 cube5 \
	dither dkong1 dng dnguy document \
	easel editor editot eye_bld1 eye_bld2 \
	eye_bld3 eyefrontmask eyesidemask eyesmaller file-cabinet \
	file_open file_shut file_shutb floppy floppy1 \
	folder foldercopy folderin folderkey folderlocked \
	folderout forty_five fourarrows frog1 frog2 \
	gcan_open gcan_shut ghost hammer hand \
	hand_mouse interdict letter letters martini \
	mbox-opening mbox1 mbox2 mbox_closed mbox_full \
	mbox_open mbox_zip mgr_audio mickface movie \
	movie1 movie2 msg_board msg_none msg_note \
	msg_read nobozos1 nose notes noseback nosefront \
	nosmoking null partlycloudy partlysunny ptr_laser \
	ptr_laseron ptr_tj rainy reel1 reel2 \
	reel3 reel4 screwdriver scube1 scube2 \
	scube3 scube4 scube5 se select slider_h \
	small_stop smalldriver smallspiff1 smallspiff2 smeye1 \
	smeye2 smeye3 smeye4 smileheart smouse0 \
	smouse1 smouse2 smouse3 sphere stopsign \
	stormy sun_mouse sunny thinker tombstone \
	tty umbrella verysunny veye1 veye2 \
	veye3 weird_thing yieldsign zip

all:

install:	$(INSDIR)
#		Do not install the files if INSDIR is the current directory.
#		This stuff with ReMoveThisFile is to get around symbolic links.
	rm -rf ./ReMoveThisFile
	touch $(INSDIR)/ReMoveThisFile
	if [ ! -f ./ReMoveThisFile ]; then \
		( list=`find [a-z0-9]* -print`; \
			cd $(INSDIR);  rm -f $$list ); \
		find [a-z0-9]* -print  |  cpio -pdumv $(INSDIR); \
	else \
		echo ">>  $(INSDIR) == current directory; no files copied"; \
	fi
	rm -rf $(INSDIR)/ReMoveThisFile

$(INSDIR):
	mkdir $@

clean clobber:

list:
	@for i in Makefile README $(ICONS); do \
		echo "$(START)/$$i"; \
	done
	@for i in ${SUBDIRS}; do \
		echo "${START}/$$i";\
		if [ -d $$i ]; then \
	 		cd $$i; $(MAKE) START=${START}/$$i list; cd ..;\
		fi \
	done	

cpio:
	make -s list | cpio -ocv > $(CPIO)
