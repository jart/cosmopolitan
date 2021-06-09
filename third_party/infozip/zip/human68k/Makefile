# Makefile for Zip, ZipCloak, ZipNote and ZipSplit for human68k
# Written by NIIMI Satoshi <a01309@cfi.waseda.ac.jp>
#
# 1999/09/23: Modified by Shimazaki Ryo.

ifeq "$(TARGET)" "X68030"
COPT = -m68020-40
AOPT = -m68020 -sCPU020
LDFLAGS = -L/usr/local/lib/lib060
endif

VPATH = human68k

CC = gcc2
CFLAGS = $(COPT) -I. -Wall -O2 -fomit-frame-pointer -fstrength-reduce \
	 -DASM_CRC -D__DOS_INLINE__
#LDFLAGS = -Wl,-x
LIBS = -lhmem -lttyi -lsignal

AS = g2as
ASFLAGS = $(AOPT) -1 -c4 -y -w2

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crypt.o ttyio.o \
       crc32.o human68k.o crc_68.o
OBJI = deflate.o trees.o
OBJA =
OBJU = zipfile_.o fileio_.o util_.o globals.o human68k_.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

ZIP_H = zip.h ziperr.h tailor.h human68k/osdep.h

all: zips

.SUFFIXES: _.o .o .c
.c_.o:
	$(CC) $(CFLAGS) -DUTIL -c $< -o $@
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

ZIPS = zip.x zipnote.x zipsplit.x zipcloak.x

zips: $(ZIPS)

zip.x: $(OBJZ) $(OBJI) $(OBJA)
	$(CC) $(LDFLAGS) -o $@ $(OBJZ) $(OBJI) $(OBJA) $(LIBS)
zipnote.x: $(OBJN)
	$(CC) $(LDFLAGS) -o $@ $(OBJN) $(LIBS)
zipcloak.x: $(OBJC)
	$(CC) $(LDFLAGS) -o $@ $(OBJC) $(LIBS)
zipsplit.x: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)


human68k.o: human68k/human68k.c
	$(CC) $(CFLAGS) -c -o $@ $<

human68k_.o: human68k/human68k.c
	$(CC) $(CFLAGS) -c -o $@ $< -DUTIL

#match.o: human68k/match.s
#	$(AS) $(ASFLAGS) -o $@ $<

deflate.o: human68k/deflate.s
	$(AS) $(ASFLAGS) -o $@ $<

crc_68.o: human68k/crc_68.s
	$(AS) $(ASFLAGS) -o $@ $<


clean:
	rm -f *.o $(ZIPS)

zip.bfd: $(ZIPS)
	rm -f $@
	for file in $(ZIPS); do \
		bdif -A -R uploaded/$$file $$file $@; \
	done

# rules for zip, zipnote, zipcloak, zipsplit.
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
zipup.o: human68k/zipup.h

# EOF
