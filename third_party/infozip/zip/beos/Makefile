######################################################################
#
# Makefile for Info-ZIP's zip, zipcloak, zipnote, and zipsplit on BeOS
#
# Copyright © 1999 Info-ZIP
#             Chris Herborth (chrish@pobox.com)
#
# This is the new New and Improved Makefile for BeOS; it automatically
# detects your platform and uses the appropriate compiler and compiler
# flags.

######################################################################
# Things that don't change:

# Punish people who don't have SMP hardware.
MAKE  = make -j 4 -f beos/Makefile
SHELL = /bin/sh

LN = ln -s

BIND = $(CC)
AS   = $(CC) -c
CPP  = $(CC) -E

INSTALL = install

# Target directories
prefix = /boot/home/config
BINDIR = $(prefix)/bin
manext = 1
MANDIR = $(prefix)/man/man$(manext)
ZIPMANUAL = MANUAL

VERSION = Version 2.3 of __DATE__

######################################################################
# Things that change:

# PowerPC system
ifeq "$(BE_HOST_CPU)" "ppc"

CC:=mwcc

ifeq "$(shell uname -r)" "4.0"

CFLAGS:=-O7 -opt schedule604 -rostr -w9 \
		-I. -DHAVE_DIRENT_H -DPASSWD_FROM_STDIN
LFLAGS1:=-warn

else

CFLAGS:=-O7 -proc 604e -w9 -I. -DHAVE_DIRENT_H -DPASSWD_FROM_STDIN
LFLAGS1:=-nodup

endif

LFLAGS2:=-L/boot/develop/lib/ppc -lbe -lroot
OBJA =
TARGET=$(ZIPS)

# x86 system
else

CC:=gcc

# Removed -Wconversion and -Wshadow because of the unnecessary warnings 
# they generate. - Sept. 28, 1999
CFLAGS:=-O3 -mpentiumpro \
		-Wall -Wno-multichar -Wno-ctor-dtor-privacy \
		-Wbad-function-cast -Woverloaded-virtual \
		-I. -I/boot/develop/headers/be/support \
	    -I/boot/develop/headers/be/storage \
	    -DHAVE_DIRENT_H -DPASSWD_FROM_STDIN # -DASMV
LFLAGS1:=
LFLAGS2:=-L/boot/develop/lib/x86 -lbe -lroot
OBJA = #match.o
TARGET=$(ZIPS)

endif

######################################################################
# Helpful targets
all:
	$(MAKE) CC=$(CC) CFLAGS="$(CFLAGS)" \
		LFLAGS1="$(LFLAGS1)" LFLAGS2="$(LFLAGS2)" \
		$(TARGET)

######################################################################
# Object file lists and other build goodies

# Object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crypt.o ttyio.o \
       beos.o crc32.o
OBJI = deflate.o trees.o
# OBJA moved into ifeq block above; we'll use assembly for x86
OBJU = zipfile_.o fileio_.o util_.o globals.o beos_.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

# Headers
ZIP_H = zip.h ziperr.h tailor.h beos/osdep.h

# What to build?
ZIPS = zip zipnote zipsplit zipcloak

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	rm -f $*_.c; $(LN) $< $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	rm -f $*_.c

.c.o:
	$(CC) -c $(CFLAGS) $<

.1.doc:
	groff -man -Tascii $< > $@

# rules for zip, zipnote, zipcloak, zipsplit, and the Zip MANUAL.
$(OBJZ): $(ZIP_H)
$(OBJI): $(ZIP_H)
$(OBJN): $(ZIP_H)
$(OBJS): $(ZIP_H)
$(OBJC): $(ZIP_H)
zip.o crc32.o crypt.o fileio.o zipfile.o zipup.o: crc32.h
zipcloak.o crc32_.o crypt_.o fileio_.o zipfile_.o: crc32.h
zip.o zipup.o crypt.o ttyio.o zipcloak.o crypt_.o: crypt.h
zip.o zipup.o zipnote.o zipcloak.o zipsplit.o: revision.h
zip.o crypt.o ttyio.o zipcloak.o crypt_.o: ttyio.h
zipup.o: beos/zipup.h

match.o: match.S
	$(CPP) match.S > _match.s
	$(AS) _match.s
	mv -f _match.o match.o
	rm -f _match.s

beos.o: beos/beos.c
	$(CC) -c $(CFLAGS) beos/beos.c

beos_.o: beos/beos.c
	rm -f $*_.c; $(LN) beos/beos.c $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	rm -f $*_.c

zips: $(ZIPS)
zipsman: $(ZIPS) $(ZIPMANUAL)

zip: $(OBJZ) $(OBJI) $(OBJA)
	$(BIND) -o zip $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote: $(OBJN)
	$(BIND) -o zipnote $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak: $(OBJC)
	$(BIND) -o zipcloak $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit: $(OBJS)
	$(BIND) -o zipsplit $(LFLAGS1) $(OBJS) $(LFLAGS2)

$(ZIPMANUAL): man/zip.1
	groff -man -Tascii man/zip.1 > $(ZIPMANUAL)

# install
install:        $(ZIPS)
	$(INSTALL) -m755 $(ZIPS) $(BINDIR)
	mkdir -p $(MANDIR)
	$(INSTALL) -m644 man/zip.1 $(MANDIR)/zip.$(manext)

uninstall:
	-cd $(BINDIR); rm -f $(ZIPS)
	-cd $(MANDIR); rm -f zip.$(manext)

dist: $(ZIPMANUAL)
	zip -u9T zip`sed -e '/VERSION/!d' -e 's/.*"\(.*\)".*/\1/' \
			  -e s/[.]//g -e q revision.h` \
	  `awk '/^Makefile/,/vms_zip.rnh/ {print $$1}' < contents`

# clean up after making stuff and installing it
clean:
	rm -f *.o $(ZIPS) flags

# end of Makefile
