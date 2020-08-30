#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.2 88/07/21 14:11:57 sau Exp $
#	$Source: /tmp/mgrsrc/font-32/RCS/Makefile,v $

#	fonts for mgr

INSDIR=$(INSROOT)/font
START=.
CPIO=font.cpio
FONTS= \
	14x25b.fnt 14x25r.fnt 80x2.fnt 81.fnt \
	bold8x16.fnt child11x15.fnt child11x15b.fnt child11x15bI.fnt \
	child11x15bu.fnt cmr10x16b.fnt cmr10x16bI.fnt cmr10x16bu.fnt \
	cmr14x25r.fnt cmr14x25rI.fnt cmr14x25ru.fnt cmr15x25r.fnt \
	cmr15x25rI.fnt cmr15x25ru.fnt cmr9x16r.fnt cmr9x16rI.fnt \
	cmr9x16ru.fnt cmrb8.fnt colossus12x20.fnt cour16x23b.fnt \
	cour16x23r.fnt cour18x26b.fnt cour18x26r.fnt cour21x31b.fnt \
	cour21x31r.fnt cour21x34b.fnt cour21x34r.fnt cour26x37b.fnt \
	cour26x37r.fnt cour28x41b.fnt cour28x41r.fnt cour32x44b.fnt \
	cour32x44r.fnt cour36x52b.fnt cour36x52r.fnt cour41x59b.fnt \
	cour41x59r.fnt cour48x66b.fnt cour48x66r.fnt cour53x74b.fnt \
	cour53x74r.fnt cour58x81b.fnt cour58x81r.fnt cour65x89b.fnt \
	cour65x89r.fnt cour6x12r.fnt cour6x12rI.fnt cour6x12ru.fnt \
	cour6x12up.fnt cour75x103b.fnt cour75x103r.fnt cour7x12b.fnt \
	cour7x12bI.fnt cour7x12bu.fnt cour7x14b.fnt cour7x14bI.fnt \
	cour7x14bu.fnt cour7x14r.fnt cour7x14rI.fnt cour7x14ru.fnt \
	cour8x16r.fnt cour8x16rI.fnt cour8x16ru.fnt cour95x132b.fnt \
	cour95x132r.fnt cour9x16b.fnt cour9x16bI.fnt cour9x16bu.fnt \
	default.fnt dot1x1r.fnt gal12x20b.fnt gal12x20r.fnt \
	gal12x20rI.fnt gal12x20ru.fnt gal13x20b.fnt gal13x20r.fnt \
	gal7x11r.fnt gal7x11rI.fnt gal7x11ru.fnt gal7x14r.fnt \
	gal7x14rI.fnt gal7x14ru.fnt gal8x14b.fnt gal8x14bI.fnt \
	gal8x14bu.fnt gal8x16b.fnt gal8x16bI.fnt gal8x16bu.fnt \
	gal9x16r.fnt gal9x16rI.fnt gal9x16ru.fnt gleek8x16.fnt \
	greek.8x16.fnt greekdown.fnt greekup.fnt italdown.fnt \
	italup.fnt ngal8x16b.fnt ngal8x16r.fnt ngal8x16rI.fnt \
	ngal8x16ru.fnt ngal9x16b.fnt ngal9x16r.fnt ngal9x16rI.fnt \
	ngal9x16ru.fnt nser8x16b.fnt nser8x16r.fnt nser8x16rI.fnt \
	nser8x16ru.fnt nser9x16b.fnt nser9x16r.fnt nser9x16rI.fnt \
	nser9x16ru.fnt oldeng22x30r.fnt oldeng22x30rI.fnt oldeng22x30ru.fnt \
	sail6x8r.fnt sail6x8rI.fnt sail6x8ru.fnt sdh9x12b.fnt \
	sdh9x12bI.fnt sdh9x12bu.fnt ser7x12r.fnt ser7x12rI.fnt \
	ser7x12ru.fnt ser7x13r.fnt ser7x13rI.fnt ser7x13ru.fnt \
	ser7x14r.fnt ser7x14rI.fnt ser7x14ru.fnt ser8x16r.fnt \
	ser8x16rI.fnt ser8x16ru.fnt ser9x18r.fnt ser9x18rI.fnt \
	ser9x18ru.fnt shadow38x40b.fnt shadow38x40bI.fnt shadow38x40bu.fnt \
	smcap8x16.fnt special8x16.fnt subscr8x16.fnt super8x16.fnt upside8x12.fnt

all:

install:	$(INSDIR)
#		Do not install the files if INSDIR is the current directory.
#		This stuff with ReMoveThisFile is to get around symbolic links.
	rm -rf ./ReMoveThisFile
	touch $(INSDIR)/ReMoveThisFile
	if [ ! -f ./ReMoveThisFile ]; then \
		( list=`echo .mgrc *.fnt`;  cd $(INSDIR);  rm -f $$list ); \
		cp .mgrc *.fnt $(INSDIR); \
	else \
		echo ">>  $(INSDIR) == current directory; no files copied"; \
	fi
	rm -rf $(INSDIR)/ReMoveThisFile

$(INSDIR):
	mkdir $@

clean clobber:

list:
	@for i in Makefile .mgrc $(FONTS); do \
		echo "${START}/$$i"; \
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}
