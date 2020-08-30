#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.10 88/07/21 12:50:30 sau Exp $
#	$Source: /tmp/mgrsrc/doc/RCS/Makefile,v $

#	mgr library documentation

INSDIR=$(INSROOT)/man

FILES1 = \
	bounce.1 browse.1 bury.1 c_menu.1 clock.1 \
	clock2.1 close.1 color.1 cut.1 cycle.1 \
	dmgr.1 ether.1 font.1 iconmail.1 iconmsgs.1 \
	ify.1 invert_colormap.1 lock.1 maze.1 menu.1 \
	mgr.1 mgrmail.1 mgrmsgs.1 oclose.1 omgrmail.1 \
	overlay.1 pilot.1 rotate.1 set_colormap.1 set_console.1 \
	set_termcap.1 setname.1 shape.1 show.1 snap.1 \
	spot.1 startup.1 stat.1 stringart.1 tjfilter.1 \
	vi.1 window_print.1 zoom.1 pgs.1 

FILES5 = bitmap.5 font.5
PRINT = hproff

FILES= $(FILES1) $(FILES5)
START=.
CPIO=doc.cpio

MAKE=make

all small:

print:	$(FILES)
	$(PRINT) -man $(FILES)
	cd usrman;  $(MAKE) PRINT=$(PRINT) all

smallinstall:	install

install:
	@echo "NOTICE:"
	@echo " The man pages are now installed with 'make installman'"
	@echo " at the top level"

old_install:	$(INSDIR) $(INSDIR)/man1 $(INSDIR)/man5
#		Do not install the files if INSDIR is the current directory.
#		This stuff with ReMoveThisFile is to get around symbolic links.
	rm -rf ./ReMoveThisFile
	touch $(INSDIR)/ReMoveThisFile
	if [ ! -f ./ReMoveThisFile ]; then \
		(cd $(INSDIR)/man1;  rm -f $(FILES1)); \
		 cp $(FILES1) $(INSDIR)/man1; \
		(cd $(INSDIR)/man5;  rm -f $(FILES5)); \
		 cp $(FILES5) $(INSDIR)/man5; \
	else \
		echo ">>  $(INSDIR) == current directory; no files copied"; \
	fi
	rm -rf $(INSDIR)/ReMoveThisFile
	cd usrman; $(MAKE) install


$(INSDIR) $(INSDIR)/man1 $(INSDIR)/man5:
	mkdir $@

clean clobber:
	cd usrman; $(MAKE) $@

list:
	@for i in Makefile $(FILES); do \
		echo "$(START)/$$i"; \
	done
	@echo "$(START)/usrman";
	@if [ -d usrman ] ; then \
		cd usrman; $(MAKE) START=$(START)/usrman list; cd ..; \
	fi

cpio:
	$(MAKE) -s list | cpio -ocv > $(CPIO)
