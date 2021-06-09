# Makefile for Zip, ZipNote, ZipCloak and ZipSplit

MAKE = make
SHELL = /bin/sh

# (to use the Gnu compiler, change cc to gcc in CC and BIND)
CC = cc
BIND = $(CC)
AS = $(CC) -c
E =
CPP = /lib/cpp

# probably can change this to 'install' if you have it
INSTALL = cp

# target directories - where to install executables and man pages to
BINDIR = /usr/local/bin
manext=1
MANDIR = /usr/local/man/man$(manext)

# flags
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)
CFLAGS = -O
LFLAGS1 =
LFLAGS2 = -s

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o crc32.o globals.o \
	crypt.o ttyio.o atari.o

OBJI = deflate.o trees.o
OBJA =
OBJU = zipfile_.o fileio_.o util_.o globals.o atari_.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

ZIP_H = zip.h ziperr.h tailor.h atari/osdep.h

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	rm -f $*_.c; ln $< $*_.c
	$(CC) $(CFLAGS) -DUTIL -c $*_.c
	rm -f $*_.c
.c.o:
	$(CC) $(CFLAGS) -c $<

.1.doc:
	nroff -man $< | col -b | uniq > $@

# rules for zip, zipnote, zipcloak, zipsplit, and the Zip MANUAL.
$(OBJZ): $(ZIP_H)
$(OBJI): $(ZIP_H)
$(OBJN): $(ZIP_H)
$(OBJS): $(ZIP_H)
$(OBJC): $(ZIP_H)
zip.o crc32.o crypt.o fileio.o zipfile.o zipup.o: crc32.h
zipcloak.o crc32_.o crypt_.o fileio_.o zipfile_.o:  crc32.h
zip.o zipup.o crypt.o ttyio.o zipcloak.o crypt_.o: crypt.h
zip.o zipup.o zipnote.o zipcloak.o zipsplit.o: revision.h
zip.o crypt.o ttyio.o zipcloak.o crypt_.o: ttyio.h
zipup.o: atari/zipup.h

match.o: match.s
	$(CPP) match.s > _match.s
	$(AS) _match.s
	mv _match.o match.o
	rm -f _match.s

ZIPS = zip$E zipnote$E zipsplit$E zipcloak$E

zips: $(ZIPS)
zipsman: $(ZIPS) $(ZIPMANUAL)

zip$E: $(OBJZ) $(OBJI) $(OBJA)
	$(BIND) -o zip$E $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote$E: $(OBJN)
	$(BIND) -o zipnote$E $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak$E: $(OBJC)
	$(BIND) -o zipcloak$E $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit$E: $(OBJS)
	$(BIND) -o zipsplit$E $(LFLAGS1) $(OBJS) $(LFLAGS2)

$(ZIPMANUAL): man/zip.1
	nroff -man man/zip.1 | col -b | uniq > $(ZIPMANUAL)

# install
install:        $(ZIPS)
	$(INSTALL) $(ZIPS) $(BINDIR)
	$(INSTALL) man/zip.1 $(MANDIR)/zip.$(manext)

uninstall:
	-cd $(BINDIR); rm -f $(ZIPS)
	-cd $(MANDIR); rm -f zip.$(manext)

dist: $(ZIPMANUAL)
	zip -u9T zip`sed -e '/VERSION/!d' -e 's/.*"\(.*\)".*/\1/' \
			  -e s/[.]//g -e q revision.h` \
	  `awk '/^Makefile/,/vms_zip.rnh/ {print $$1}' < contents`

# ATARI version (gcc 2.5.8 and Mintlibs PL46)
atari:
	$(MAKE) zips CFLAGS="-O -DATARI" OBJA=atari/atari.o CC=gcc E=.ttp

# clean up after making stuff and installing it
clean:
	rm -f *.o $(ZIPS) flags
