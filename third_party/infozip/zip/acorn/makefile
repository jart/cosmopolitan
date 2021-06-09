# Makefile for Zip, ZipNote, ZipCloak and ZipSplit

# add -g to CC to debug
# add -d to BIND to debug
CC   = cc
BIND = link
AS   = $(CC) -c
ASM  = objasm
SQUEEZE = squeeze -v
E    =

# flags
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)
#
LIB      =
CBASE    = -throwback -wn -DASMV -apcs 3/26
CFLAGS   = $(CBASE) -IC:,@.
ASMFLAGS = -Throwback -Stamp -NoCache -CloseExec -quit -apcs 3/26
LFLAGS1  =
LFLAGS2  = $(LIB) C:o.Stubs

# Uncomment the following line to enable support for Unix
# Extra Field (Timezone)
#CFLAGS  = $(CFLAGS) -DUSE_EF_UT_TIME

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crc32.o \
       crypt.o ttyio.o riscos.o acornzip.o swiven.o

OBJI = deflate.o trees.o
OBJA = match.o sendbits.o
OBJU = zipfile_.o fileio_.o util_.o globals.o riscos.o acornzip_.o swiven.o
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) crc32_.o crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

ZIP_H = zip.h ziperr.h tailor.h acorn/osdep.h acorn/riscos.h acorn/swiven.h

all:      zip zipnote zipsplit zipcloak


install:  %.zip %.zipnote %.zipsplit %.zipcloak %.zipsfx \
          zip zipnote zipsplit zipcloak zipsfx
        $(SQUEEZE) zip %.zip
        $(SQUEEZE) zipnote %.zipnote
        $(SQUEEZE) zipsplit %.zipsplit
        $(SQUEEZE) zipcloak %.zipcloak
        copy acorn.zipsfx %.zipsfx ~CVF

# suffix rules
.SUFFIXES:      _.o .o .c
.c_.o:
        $(CC) $(CFLAGS) -DUTIL -c $*.c -o $*_.o
.c.o:
        $(CC) $(CFLAGS) -c $<
.s.o:
        $(ASM) $(ASMFLAGS) -from @*.s -to @*.o

# rules for zip, zipnote, zipcloak and zipsplit
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

crc32_.o: crc32.c
        $(CC) $(CFLAGS) -DUTIL -c c.crc32 -o o.crc32_
crypt_.o: crypt.c
        $(CC) $(CFLAGS) -DUTIL -c c.crypt -o o.crypt_
util_.o: util.c
        $(CC) $(CFLAGS) -DUTIL -c c.util -o o.util_
fileio_.o: fileio.c
        $(CC) $(CFLAGS) -DUTIL -c c.fileio -o o.fileio_
zipfile_.o: zipfile.c
        $(CC) $(CFLAGS) -DUTIL -c c.zipfile -o o.zipfile_
acornzip_.o: acorn/acornzip.c $(ZIP_H)
        $(CC) $(CFLAGS) -DUTIL -c acorn/acornzip.c -o o.acornzip_

riscos.o: acorn/riscos.c acorn/riscos.h
        $(CC) $(CFLAGS) -c acorn/riscos.c

acornzip.o: acorn/acornzip.c $(ZIP_H)
        $(CC) $(CFLAGS) -c acorn/acornzip.c

match.o: acorn/match.s
        $(ASM) $(ASMFLAGS) -from acorn.s.match -to o.match

sendbits.o: acorn/sendbits.s
        $(ASM) $(ASMFLAGS) -from acorn.s.sendbits -to o.sendbits

swiven.o: acorn/swiven.s
        $(ASM) $(ASMFLAGS) -from acorn.s.swiven -to o.swiven

zip:      $(OBJZ) $(OBJI) $(OBJA)
          $(BIND) -o zip$(E) $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote:  $(OBJN)
          $(BIND) -o zipnote$(E) $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak: $(OBJC)
          $(BIND) -o zipcloak$(E) $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit: $(OBJS)
          $(BIND) -o zipsplit$(E) $(LFLAGS1) $(OBJS) $(LFLAGS2)

clean:  ;remove zip; remove zipcloak;
        remove zipsplit; remove zipnote;
        create o.!fake! 0
        wipe o.* ~cf

# end of Makefile
