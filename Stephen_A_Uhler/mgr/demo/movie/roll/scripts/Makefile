#		Makefile for roll demo scripts

START=.

all install clean clobber:

list:
	@for i in *; do \
		echo "${START}/$$i";\
	done	

cpio:
	make -s list | cpio -ocv > ${CPIO}

