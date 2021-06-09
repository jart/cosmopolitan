# WMAKE makefile for 16 bit MSDOS or 32 bit DOS extender (PMODE/W or DOS/4GW)
# using Watcom C/C++ v11.0+, by Paul Kienitz, last revised 07 Aug 2005.
# Makes Zip.exe, ZipNote.exe, ZipCloak.exe, and ZipSplit.exe.
#
# Invoke from Zip source dir with "WMAKE -F MSDOS\MAKEFILE.WAT [targets]"
# To build with debug info use "WMAKE DEBUG=1 ..."
# To build with no assembly modules use "WMAKE NOASM=1 ..."
# To make the PMODE/W version use "WMAKE PM=1 ..."
# To make the DOS/4GW version use "WMAKE GW=1 ..." (overrides PM=1)
#   Note: specifying PM or GW without NOASM requires that the win32 source
#   directory be present, so it can access the 32 bit assembly sources.
#   PMODE/W is recommended over DOS/4GW for best performance.
# To create a low memory usage version of Zip, use "WMAKE WSIZE=8192 ..."
#   (or whatever power of two less than 32768 you wish) -- this also causes
#   SMALL_MEM to be defined.  Compression performance will be reduced.
#   This currently is not supported with PM=1 or GW=1.
#
# Other options to be fed to the compiler and assembler can be specified in
# an environment variable called LOCAL_ZIP.

variation = $(%LOCAL_ZIP)

# Stifle annoying "Delete this file?" questions when errors occur:
.ERASE

.EXTENSIONS:
.EXTENSIONS: .exe .obj .c .h .asm

# We maintain multiple sets of object files in different directories so that
# we can compile msdos, dos/4gw or pmode/w, and win32 versions of Zip without
# their object files interacting.  The following var must be a directory name
# ending with a backslash.  All object file names must include this macro
# at the beginning, for example "$(O)foo.obj".

!ifdef GW
PM = 1      # both protected mode formats use the same object files
!endif

!ifdef DEBUG
!  ifdef PM
OBDIR = od32d
!  else
!    ifdef WSIZE
OBDIR = od16l
size = -DWSIZE=$(WSIZE) -DSMALL_MEM
!    else
OBDIR = od16d
size = -DMEDIUM_MEM
!    endif
!  endif
!else
!  ifdef PM
OBDIR = ob32d
!  else
!    ifdef WSIZE
OBDIR = ob16l
size = -DWSIZE=$(WSIZE) -DSMALL_MEM
!    else
OBDIR = ob16d
size = -DMEDIUM_MEM
!    endif
!  endif
!endif
O = $(OBDIR)\   # comment here so backslash won't continue the line

# The assembly hot-spot code in crc_i[3]86.asm and match[32].asm is
# optional.  This section controls its usage.

!ifdef NOASM
  # C source
asmob =
cvars = $+$(cvars)$- -DDYN_ALLOC -DNO_ASM   # or ASM_CRC might default on!
# "$+$(foo)$-" means expand foo as it has been defined up to now; normally,
# this make defers inner expansion until the outer macro is expanded.
!else  # !NOASM
asmob = $(O)crc.obj $(O)match.obj
!  ifdef PM
cvars = $+$(cvars)$- -DASM_CRC -DASMV       # no DYN_ALLOC with match32.asm
crc_s = win32\crc_i386.asm   # requires that the win32 directory be present
mat_s = win32\match32.asm    # ditto
!  else
cvars = $+$(cvars)$- -DDYN_ALLOC -DASM_CRC -DASMV
avars = $+$(avars)$- -DDYN_ALLOC
crc_s = msdos\crc_i86.asm
mat_s = msdos\match.asm
!  endif
!endif

# Now we have to pick out the proper compiler and options for it.  This gets
# pretty complicated with the PM, GW, DEBUG, and NOASM options...

link   = wlink
asm    = wasm

!ifdef PM
cc     = wcc386
# Use Pentium Pro timings, register args, static strings in code:
cflags = -bt=DOS -mf -6r -zt -zq
aflags = -bt=DOS -mf -3 -zq
cvars  = $+$(cvars)$- -DDOS $(variation)
avars  = $+$(avars)$- -DWATCOM_DSEG $(variation)

!  ifdef GW
lflags = sys DOS4G
!  else
# THIS REQUIRES THAT PMODEW.EXE BE FINDABLE IN THE COMMAND PATH.
# It does NOT require you to add a pmodew entry to wlink.lnk or wlsystem.lnk.
defaultlibs = libpath %WATCOM%\lib386 libpath %WATCOM%\lib386\dos
lflags = format os2 le op osname='PMODE/W' op stub=pmodew.exe $(defaultlibs)
!  endif

!else   # plain 16-bit DOS:

cc     = wcc
# Use plain 8086 instructions, large memory model, static strings in code:
cflags = -bt=DOS -ml -0 -zt -zq
aflags = -bt=DOS -ml -0 -zq
cvars  = $+$(cvars)$- -DDOS $(size) $(variation)
avars  = $+$(avars)$- $(size) $(variation)
lflags = sys DOS
!endif  # !PM

# Specify optimizations, or a nonoptimized debugging version:

!ifdef DEBUG
cdebug = -od -d2
ldebug = d w all op symf
!else
!  ifdef PM
cdebug = -s -obhikl+rt -oe=100 -zp8
# -oa helps slightly but might be dangerous.
!  else
cdebug = -s -oehiklrt
!  endif
ldebug = op el
!endif

# How to compile most sources:
.c.obj:
	$(cc) $(cdebug) $(cflags) $(cvars) $[@ -fo=$@

# Our object files.  OBJZ is for Zip, OBJC is for ZipCloak, OBJN is for
# ZipNote, and OBJS is for ZipSplit:

OBJZ2 = $(O)zip.obj $(O)crypt.obj $(O)ttyio.obj $(O)zipfile.obj $(O)zipup.obj
OBJZA = $(OBJZ2) $(O)util.obj $(O)fileio.obj $(O)deflate.obj
OBJZB = $(O)trees.obj $(O)globals.obj $(O)crc32.obj $(asmob) $(O)msdos.obj

OBJU2 = $(O)zipfile_.obj $(O)fileio_.obj $(O)util_.obj $(O)globals.obj
OBJ_U = $(OBJU2) $(O)msdos_.obj

OBJC  = $(O)zipcloak.obj $(O)crc32_.obj $(O)crypt_.obj $(O)ttyio.obj $(OBJ_U)

OBJN  = $(O)zipnote.obj $(OBJ_U)

OBJS  = $(O)zipsplit.obj $(OBJ_U)

# Common header files included by all C sources:

ZIP_H = zip.h ziperr.h tailor.h msdos\osdep.h


# HERE WE GO!  By default, make all targets:
all: Zip.exe ZipNote.exe ZipCloak.exe ZipSplit.exe

# Convenient shorthand options for single targets:
z:   Zip.exe       .SYMBOLIC
n:   ZipNote.exe   .SYMBOLIC
c:   ZipCloak.exe  .SYMBOLIC
s:   ZipSplit.exe  .SYMBOLIC

Zip.exe:	$(OBDIR) $(OBJZA) $(OBJZB) $(OBJV)
	set WLK_VA=file {$(OBJZA)}
	set WLK_VB=file {$(OBJZB) $(OBJV)}
	$(link) $(lflags) $(ldebug) name $@ @WLK_VA @WLK_VB
	set WLK_VA=
	set WLK_VB=
# We use WLK_VA and WLK_VB to keep the size of each command under 256 chars.

ZipNote.exe:	$(OBDIR) $(OBJN)
	set WLK_VAR=file {$(OBJN)}
	$(link) $(lflags) $(ldebug) name $@ @WLK_VAR
	set WLK_VAR=

ZipCloak.exe:	$(OBDIR) $(OBJC)
	set WLK_VAR=file {$(OBJC)}
	$(link) $(lflags) $(ldebug) name $@ @WLK_VAR
	set WLK_VAR=

ZipSplit.exe:	$(OBDIR) $(OBJS)
	set WLK_VAR=file {$(OBJS)}
	$(link) $(lflags) $(ldebug) name $@ @WLK_VAR
	set WLK_VAR=

# Source dependencies:

$(O)crc32.obj:    crc32.c $(ZIP_H) crc32.h
$(O)crypt.obj:    crypt.c $(ZIP_H) crypt.h crc32.h ttyio.h
$(O)deflate.obj:  deflate.c $(ZIP_H)
$(O)fileio.obj:   fileio.c $(ZIP_H) crc32.h
$(O)globals.obj:  globals.c $(ZIP_H)
$(O)trees.obj:    trees.c $(ZIP_H)
$(O)ttyio.obj:    ttyio.c $(ZIP_H) crypt.h ttyio.h
$(O)util.obj:     util.c $(ZIP_H)
$(O)zip.obj:      zip.c $(ZIP_H) crc32.h crypt.h revision.h ttyio.h
$(O)zipfile.obj:  zipfile.c $(ZIP_H) crc32.h
$(O)zipup.obj:    zipup.c $(ZIP_H) revision.h crc32.h crypt.h msdos\zipup.h
$(O)zipnote.obj:  zipnote.c $(ZIP_H) revision.h
$(O)zipcloak.obj: zipcloak.c $(ZIP_H) revision.h crc32.h crypt.h ttyio.h
$(O)zipsplit.obj: zipsplit.c $(ZIP_H) revision.h

# Special case object files:

$(O)msdos.obj:    msdos\msdos.c $(ZIP_H)
	$(cc) $(cdebug) $(cflags) $(cvars) msdos\msdos.c -fo=$@

$(O)match.obj:    $(mat_s)
	$(asm) $(aflags) $(avars) $(mat_s) -fo=$@

$(O)crc.obj:      $(crc_s)
	$(asm) $(aflags) $(avars) $(crc_s) -fo=$@

# Variant object files for ZipNote, ZipCloak, and ZipSplit:

$(O)zipfile_.obj: zipfile.c $(ZIP_H) crc32.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL zipfile.c -fo=$@

$(O)fileio_.obj:  fileio.c $(ZIP_H) crc32.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL fileio.c -fo=$@

$(O)util_.obj:    util.c $(ZIP_H)
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL util.c -fo=$@

$(O)crc32_.obj:   crc32.c $(ZIP_H) crc32.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL crc32.c -fo=$@

$(O)crypt_.obj:   crypt.c $(ZIP_H) crc32.h crypt.h ttyio.h
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL crypt.c -fo=$@

$(O)msdos_.obj:   msdos\msdos.c $(ZIP_H)
	$(cc) $(cdebug) $(cflags) $(cvars) -DUTIL msdos\msdos.c -fo=$@

# Creation of subdirectory for intermediate files
$(OBDIR):
	-mkdir $@

# Unwanted file removal:

clean:     .SYMBOLIC
	del $(O)*.obj

cleaner:   clean  .SYMBOLIC
	del Zip.exe
	del ZipNote.exe
	del ZipCloak.exe
	del ZipSplit.exe
