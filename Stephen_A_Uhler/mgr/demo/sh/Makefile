#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.3 88/08/12 07:42:55 sau Exp $
#	$Source: /tmp/mgrsrc/demo/sh/RCS/Makefile,v $

# makefile for shell files 

INSROOT=/usr/mgr
INSDIR=$(INSROOT)/bin

ALL = \
	bury c_menu color \
	cursor font loadfont \
	menu more resetwindow \
	shape showfont square vi

START = .

all small fast:	$(ALL)


install:	$(INSDIR)
		-cp $(ALL) $(INSDIR)
		cd $(INSDIR);  chmod 755 $(ALL)

$(INSDIR):
		mkdir $@

clean clobber:

list:
	@for i in $(ALL) Makefile; do \
		echo "$(START)/$$i"; \
	done	
