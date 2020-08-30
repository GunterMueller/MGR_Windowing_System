#                        Copyright (c) 1987 Bellcore
#                            All Rights Reserved
#       Permission is granted to copy or use this program, EXCEPT that it
#       may not be sold for profit, the copyright notice must be reproduced
#       on copies, and credit should be given to Bellcore where it is due.
#       BELLCORE MAKES NO WARRANTY AND ACCEPTS NO LIABILITY FOR THIS PROGRAM.

#	$Header: Makefile,v 1.1 89/09/27 09:55:04 sau Exp $
#	$Source: /m1/mgr.new/RCS/Makefile,v $

#	top level makefile for mgr

#########################################################################

#	To make mgr type:
#	make INSROOT=<root> TARGET=<target> [FLAG=<compile_flag>]
#		<root> is the mgr root directory (normally /usr/mgr)
#		<target> is the target version of mgr, typically
#			prodmgr:	 for the "production" monochrome release or
#			colormgr: for the color version
#		<compile_flag> is passwd to cc, such as -O or -G
#

#########################################################################33


# set to root of mgr directory
INSROOT = /usr/mgr

# pieces necessary for creating demos and applications only
# (for not-necessarily-suns)
SMALLDIR = lib demo doc menu

# directories that need making (-src, a special case)
SUBDIR = font icon $(SMALLDIR)

# full mgr directories
ALLDIR = src lib demo misc font icon doc

START=.

CPIO=cpio
CPIO_OUT=mgr.cpio
CPIO_OPTS = ocv

MAKE=make

all:	$(ALLDIR)

default:
	@echo "Makeing default MGR"
	$(MAKE) FLAG=$(FLAG) TARGET=portmgr INSROOT=/usr/mgr all

#	"make small" builds the mgr support library and support commands.
#	A "small" mgr package (see smallinstall and smallcpio) is that portion
#	which does not necessarily have to be on the same machine as the mgr
#	command.

small:
	for i in $(SMALLDIR); do \
	(cd $$i; echo "	making $$i";\
 	 $(MAKE)  INSROOT=$(INSROOT) $@;\
	 cd ..);\
	done	

#	"make <<directoryname>>" will make the named directory.  Note that this
#	is the better way to make individual subdirectories because that way
#	INSROOT is propagated down to all files that need it, in particular
#	"src" files.

$(SUBDIR):
	cd $@;  echo "	making $@";  $(MAKE) \
		 OTHERLIB=$(OTHERLIB) INSROOT=$(INSROOT) FLAG=$(FLAG) TARGET=$(TARGET) all

src:
	cd src; echo " making src"; $(MAKE) \
		 OTHERLIB=$(OTHERLIB) INSROOT=$(INSROOT) $(TARGET) FLAG=$(FLAG)

#	"make install" installs mgr and all its support files and commands.
#	mgr is not made setuid-root.

install:	$(INSROOT)
	for i in $(ALLDIR); do \
	(cd $$i; echo "	installing $$i";\
 	 $(MAKE) INSROOT=$(INSROOT) install;\
	 cd ..);\
	done	
	date > $(INSROOT)/.installed

#	"make installasroot" runs "make install" as root.  mgr and some of the
#	support commands are made setuid-root.
installasroot:
	/bin/su root -c '$(MAKE) OTHERLIB=$(OTHERLIB) INSROOT=$(INSROOT) \
		TARGET=$(TARGET) install'

#	"make smallinstall" installs the mgr support commands which do not
#	have to be on the same machine as the mgr command.

smallinstall:	$(INSROOT)
	for i in $(SMALLDIR); do \
		(cd $$i; echo "	$@ing $$i";\
		 $(MAKE)  OTHERLIB=$(OTHERLIB) INSROOT=$(INSROOT) $@;\
		 cd ..);\
	done	

$(INSROOT):
	mkdir $@

#	"make clean" removes all intermediate files created by "make all".
#	"make clobber" removes all target files created by "make all".

clean clobber:
	for i in $(ALLDIR); do \
		if [ -d $$i ]; then \
			(cd $$i; echo "	$@ing $$i";\
			 $(MAKE)  INSROOT=$(INSROOT) $@;\
			 cd ..);\
		fi; \
	done	

#	"make list" outputs the complete list of files needed by "make all".
#	This list is also what goes into a delivery package.
#	This list is used by several scripts to keep track of stuff

list:
	@echo $(START)/README
	@echo $(START)/README.old
	@echo $(START)/Makefile
	@/bin/ls -1 $(START)/PATCH*
	@for i in $(ALLDIR); do \
		echo "$(START)/$$i"; \
		if [ -d $$i ]; then \
			cd $$i; $(MAKE)  START=$(START)/$$i list; cd ..; \
		fi \
	done	


#	"make smalllist" outputs the list of files needed by "make small".
#	This list is also what goes into a "small" delivery package.

smalllist:
	@echo $(START)/README
	@echo $(START)/Makefile
	@echo $(START)/font
	@echo $(START)/font/default.fnt
	@echo $(START)/font/Makefile
	@for i in $(SMALLDIR); do \
		(cd $$i; echo "$(START)/$$i";\
		 $(MAKE)  START=$(START)/$$i list;\
		 cd ..);\
	done	

#	"make cpio" creates a file containing a complete delivery package.
cpio:
	$(MAKE) -s list | $(CPIO) -$(CPIO_OPTS) > $(CPIO_OUT)

#	"make smallcpio" creates a file containing a "small" delivery package.
smallcpio:
	$(MAKE) -s smalllist | $(CPIO) -$(CPIO_OPTS) > $(CPIO_OUT).small

#	make installman installs all the manuals (what a hack!)

installman:
	-mkdir $(INSROOT)/man $(INSROOT)/man/man1 $(INSROOT)/man/man5
	-find . -name '*.1' -print -exec cp {} $(INSROOT)/man/man1 \;
	cp doc/*.5 $(INSROOT)/man/man5

#	The FRC target forces items to remake.  Thus "make lib" will
#	issue the commands for the "lib" target, even though the lib directory
#	exists.

src $(SUBDIR):	FRC

FRC:

#	for GNU make
.PHONY:	src $(SUBDIR)
.PHONY:	clean list fast cpio smallcpio install smallinstall
