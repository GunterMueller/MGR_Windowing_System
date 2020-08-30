#                        Copyright (c) 1988 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 4.7 88/07/21 12:51:08 sau Exp $
#	$Source: /tmp/mgrsrc/doc/usrman/RCS/Makefile,v $

#		manual makefile

#	Directory to put the intermediate files
TEMPDIR=.
TEMPDIR=/tmp

#name of destination printer, as in $(TROFF) -T$(PRINTER) ...
TYPE=psc

#name of printer, as in lpr -P(printer)
PRINTER=ps16

#	set to restrict range of printed pages
RANGE=

#	Name of troff program 
TROFF=ditroff

#	Name of troff print filter, that troffs and routes to a printer,
#	all in one
#name of temp file
PRINT=psroff

#	where troff wants the fonts
FONT=-F/usr/local/psc/font

TEMP=$(TEMPDIR)/mgrman.$(PRINTER)

# Files needed to build manual
PARTS= doc.0a doc.1a doc.2a doc.3a doc.4a doc.5a doc.6a $(TEMPDIR)/doc.7a $(TEMPDIR)/doc.8a doc.9a

# files used to extract index 
EXTRACT= doc.0a doc.1a doc.2a doc.3a doc.4a doc.5a doc.6a

#	croff (c-source => troff input converter) directory and flags
CROFF=./croff
CRF=-k "" "" -c '\fI' '\fP'

VERSION = version

#	where to install manual
INSDIR=$(INSROOT)/doc

# what installed nroff'd manual is called
NAME=usrman.out

FILES= README INDEX abstract version $(EXTRACT) doc.9a

MAKE=make
START=.
CPIO=userman.cpio

##########################################################

all:	abs print

fast small:	all

ver:
		v=`cat $(VERSION)`;expr $$v + 1 > $(VERSION);\
			echo .nr $$v > ver

ps:	ver $(CROFF)/croff $(PARTS)
		-tbl ver $(PARTS) | $(CROFF)/croff $(CRF) | \
			$(PRINT) -t -ms $(RANGE) > ps

print:	ver $(CROFF)/croff $(PARTS)
		-tbl ver $(PARTS) | $(CROFF)/croff $(CRF) | \
			$(PRINT) -P$(PRINTER) -ms $(RANGE)

abs:	ver doc.0a abstract
		cat ver doc.0a abstract | $(PRINT) -ms

$(TEMP).index:	$(EXTRACT)
		@echo "extracting category list and index from document"
		tbl ver $(EXTRACT)| $(CROFF)/croff $(CRF) | \
			$(TROFF) $(FONT) -T$(TYPE) -ms -rI1 >/dev/null 2> $(TEMP)
		mv $(TEMP) $(TEMP).index

$(TEMPDIR)/doc.7a:	$(TEMP).index
		@echo "creating subject categories to $(TEMPDIR)/doc.7a"
		@grep "^.Fc" $(TEMP).index | \
				sort +2 -n  > $(TEMPDIR)/doc.7a

$(TEMPDIR)/doc.8a:	$(TEMP).index INDEX
		@echo "building index to $(TEMPDIR)/doc.8a"
		sh INDEX  < $(TEMP).index  > $(TEMPDIR)/doc.8a

#	nroff version
nroff:
		$(MAKE) TROFF=nroff PRINT=nroff PRINTER=$(PRINTER) all

install: $(INSDIR)
		-rm -f $(INSDIR)/$(NAME)
		-cp $(NAME) $(INSDIR)/$(NAME)
		
$(INSDIR):
	-mkdir $@

$(NAME):
	$(MAKE) -s PRINTER=crt nroff > $(NAME)

$(CROFF)/croff:
	cd $(CROFF);$(MAKE) croff clean

clean:
		rm -f $(TEMPDIR)/doc7a $(TEMPDIR)/doc8a $(TEMP).*
		cd $(CROFF); $(MAKE) clean

clobber: clean
		rm -f $(NAME)
		cd $(CROFF); $(MAKE) clobber

list:
	@for i in Makefile $(FILES) $(NAME); do \
		echo "$(START)/$$i"; \
	done	
	@echo "$(START)/croff";
	@if [ -d $(CROFF) ]; then \
		cd $(CROFF); $(MAKE) START=$(START)/$(CROFF) list; \
	fi

cpio:
	$(MAKE) -s list | cpio -ocv > $(CPIO)

.PHONY:	all print ver install clean list clobber cpio nroff
