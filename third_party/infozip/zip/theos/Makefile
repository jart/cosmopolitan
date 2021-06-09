# Makefile for Zip, ZipCloak, ZipNote and ZipSplit for
# THEOS ANSI C
# To use, do "make -f theos/makefile"
# This make file uses cl, a C compiler and linker driver written by
# Jean-Michel Dubois. Send a mail to jmdubois@ibcfrance.fr to get it for free.
# MAINWA_BUG    Workaround argument expansion failure
# LOCATE_BUG    Workaround stat, fopen and open failure on relative paths to
#               root dir.

CC=cl
CFLAGS=-Zi -W3 -DDYN_ALLOC -DCRYPT -DMAINWA_BUG -DLOCATE_BUG
LD=cl -o
LDFLAGS=-m -Zi
AS=cc
ASFLAGS=

UTILFLAGS=-DUTIL $(CFLAGS) -Fo

# variables

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crypt.o ttyio.o \
       theos.o crc32.o _fprintf.o _stat.o _chmod.o _isatty.o \
       _setargv.o _rename.o
OBJI = deflate.o trees.o
OBJA =
OBJU = zipfile_.o fileio_.o util_.o globals.o theos_.o _rename.o _stat.o \
       _chmod.o _fprintf.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

OSDEP_H = theos/osdep.h
ZIP_H = zip.h ziperr.h tailor.h $(OSDEP_H)

ZIPS = zip.command zipnote.command zipsplit.command zipcloak.command

zips:	$(ZIPS)

zip.o:	zip.c $(ZIP_H) crc32.h crypt.h ttyio.h revision.h
	$(CC) -c $(CFLAGS) $*.c

zipfile.o:	zipfile.c $(ZIP_H) crc32.h
	$(CC) -c $(CFLAGS) $*.c

zipup.o:	zipup.c $(ZIP_H) revision.h crc32.h crypt.h
	$(CC) -c $(CFLAGS) $*.c

fileio.o:	fileio.c $(ZIP_H) crc32.h
	$(CC) -c $(CFLAGS) $*.c

util.o:	util.c $(ZIP_H) theos/charconv.h
	$(CC) -c $(CFLAGS) $*.c

globals.o:	globals.c $(ZIP_H)
	$(CC) -c $(CFLAGS) $*.c

crc32.o:	crc32.c $(ZIP_H) crc32.h
	$(CC) -c $(CFLAGS) $*.c

deflate.o:	deflate.c $(ZIP_H)
	$(CC) -c $(CFLAGS) $*.c

trees.o:	trees.c $(ZIP_H)
	$(CC) -c $(CFLAGS) $*.c

crypt.o:	crypt.c $(ZIP_H) crc32.h crypt.h
	$(CC) -c $(CFLAGS) $*.c

theos.o:	theos/theos.c $(ZIP_H)
	$(CC) -c $(CFLAGS) -Fo$@ theos/theos.c

_fprintf.o:	theos/_fprintf.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_fprintf.c

_stat.o:	theos/_stat.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_stat.c

_chmod.o:	theos/_chmod.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_chmod.c

_isatty.o:	theos/_isatty.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_isatty.c

_rename.o:	theos/_rename.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_rename.c

_setargv.o:	theos/_setargv.c
	$(CC) -c $(CFLAGS) -Fo$@ theos/_setargv.c

ttyio.o:	$(ZIP_H) ttyio.h ttyio.c
	$(CC) -c $(CFLAGS) $*.c

zipcloak.o:	zipcloak.c $(ZIP_H) crc32.h crypt.h ttyio.h revision.h
	$(CC) -c $(CFLAGS) $*.c

zipnote.o:	zipnote.c $(ZIP_H) revision.h
	$(CC) -c $(CFLAGS) $*.c

zipsplit.o:	$(ZIP_H) revision.h
	$(CC) -c $(CFLAGS) $*.c

zipfile_.o:	zipfile.c $(ZIP_H) crc32.h
	$(CC) -c $(UTILFLAGS)$@ zipfile.c

fileio_.o:	fileio.c $(ZIP_H) crc32.h
	$(CC) -c $(UTILFLAGS)$@ fileio.c

theos_.o:	theos/theos.c $(ZIP_H)
	$(CC) -c $(UTILFLAGS)$@ theos/theos.c

util_.o:	util.c $(ZIP_H)
	$(CC) -c $(UTILFLAGS)$@ util.c

crc32_.o:	crc32.c $(ZIP_H) crc32.h
	$(CC) -c $(UTILFLAGS)$@ $*.c

crypt_.o:	crypt.c $(ZIP_H) crc32.h crypt.h
	$(CC) -c $(UTILFLAGS)$@ crypt.c

zip.command: $(OBJZ) $(OBJI)
	$(LD) $@ $(OBJZ) $(OBJI) $(LDFLAGS)

zipcloak.command: $(OBJC)
	$(LD) $@ $(OBJC) $(LDFLAGS)

zipnote.command: $(OBJN)
	$(LD) $@ $(OBJN) $(LDFLAGS)

zipsplit.command: $(OBJS)
	$(LD) $@ $(OBJS) $(LDFLAGS)

install:	$(ZIPS)
	copy *.command /system.cmd32.=(rep noq not

clean:
	erase *.o(noq not
	erase *.command(noq not
