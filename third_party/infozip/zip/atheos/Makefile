######################################################################
#
# Makefile for Info-ZIP's zip, zipcloak, zipnote, and zipsplit on AtheOS
#
# Copyright (C) 1999-2007 Info-ZIP
#                    Chris Herborth (chrish@pobox.com)
#                    Ruslan Nickolaev (nruslan@hotbox.ru)
#
######################################################################
# Things that don't change:

# Punish people who don't have SMP hardware.
MAKE  = make -j 4 -f atheos/Makefile
SHELL = /bin/sh

LN = ln -s
RM = rm -f

BIND = $(CC)
AS   = as

INSTALL = install

# Target directories
prefix = /usr
BINDIR = $(prefix)/bin
manext = 1
MANDIR = $(prefix)/man/man$(manext)
ZIPMANUAL = MANUAL

VERSION = Version 2.3 of __DATE__

######################################################################

CC:=gcc
CFLAGS:=-O3 -march=i586 -Wall -I. -DHAVE_DIRENT_H -DPASSWD_FROM_STDIN -DASMV -DASM_CRC
LFLAGS1:=
LFLAGS2:=
TARGET=$(ZIPS)

######################################################################
# Helpful targets
all:
	$(MAKE) CC=$(CC) CFLAGS="$(CFLAGS)" \
		LFLAGS1="$(LFLAGS1)" LFLAGS2="$(LFLAGS2)" \
		$(TARGET)

######################################################################
# Object file lists and other build goodies

# Object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crc32.o crypt.o \
       ttyio.o atheos.o
OBJI = deflate.o trees.o
OBJA = match.o crc_i386.o
OBJU = zipfile_.o fileio_.o util_.o globals.o atheos_.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

# Headers
ZIP_H = zip.h ziperr.h tailor.h atheos/osdep.h

# What to build?
ZIPS = zip zipnote zipsplit zipcloak

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	$(RM) $*_.c; $(LN) $< $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	$(RM) $*_.c

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
zipup.o: atheos/zipup.h

match.o: match.S
	$(CC) -E match.S > matchs.s
	$(AS) -o $@ matchs.s
	$(RM) matchs.s

crc_i386.o: crc_i386.S
	$(CC) -E crc_i386.S > crc_i386s.s
	$(AS) -o $@ crc_i386s.s
	$(RM) crc_i386s.s

atheos.o: atheos/atheos.c
	$(CC) -c $(CFLAGS) atheos/atheos.c

atheos_.o: atheos/atheos.c
	$(RM) $*_.c; $(LN) atheos/atheos.c $*_.c
	$(CC) -c $(CFLAGS) -DUTIL $*_.c
	$(RM) $*_.c

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
	-cd $(BINDIR); $(RM) $(ZIPS)
	-cd $(MANDIR); $(RM) zip.$(manext)

dist: $(ZIPMANUAL)
	zip -u9T zip`sed -e '/VERSION/!d' -e 's/.*"\(.*\)".*/\1/' \
			  -e s/[.]//g -e q revision.h` \
	  `awk '/^Makefile/,/vms_zip.rnh/ {print $$1}' < contents`

# clean up after making stuff and installing it
clean:
	$(RM) *.o $(ZIPS) flags

# end of Makefile
