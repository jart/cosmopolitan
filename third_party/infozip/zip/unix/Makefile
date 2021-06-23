# Makefile for Zip, ZipNote, ZipCloak and ZipSplit

# what you can make ...
all:
	@echo ''
	@echo 'Make what?  You must say what system to make Zip for--e.g.'
	@echo '"make generic".'
	@echo 'Choices: generic, generic_gcc, att6300nodir,'
	@echo 'coherent, cray_v3, cygwin, lynx, minix, os390,'
	@echo 'qnx, qnxnto, solaris, solaris_gcc'
	@echo 'Try first "make -f unix/Makefile generic" as'
	@echo 'it should autodetect and set the proper flags.'
	@echo 'To make the manuals use "make zipsman" after Zip is made.'
	@echo 'See the files INSTALL and zip.txt for more information.'
	@echo ''

list:   all

#MAKE = make -f unix/Makefile
MAKEF = -f unix/Makefile
SHELL = /bin/sh
LN = ln -s

# (to use the GNU compiler, change cc to gcc in CC)
CC = cc
BIND = $(CC)
AS = $(CC) -c
CPP = /lib/cpp
E =

# probably can change this to 'install' if you have it
INSTALL_PROGRAM = cp
# probably can change this to 'install -d' if you have it
# XXX NextStep 3.3 and Openstep 4.x don't know about -p !
INSTALL_D = mkdir -p
CHMOD = chmod
BINFLAGS = 755
MANFLAGS = 644

# target directories - where to install executables and man pages to
prefix = /usr/local
BINDIR = $(prefix)/bin
MANEXT=1
MANDIR = $(prefix)/man/man$(MANEXT)
ZIPMANUAL = zip.txt
ZIPMANUALcloak = zipcloak.txt
ZIPMANUALnote = zipnote.txt
ZIPMANUALsplit = zipsplit.txt
ZIPMANUALs = zip.txt zipcloak.txt zipnote.txt zipsplit.txt
PKGDIR = IZzip
VERSION = Version 3.0

# Our bzip2 directory
IZ_OUR_BZIP2_DIR = bzip2

# flags
#   CFLAGS    flags for C compile
#   LFLAGS1   flags after output file spec, before obj file list
#   LFLAGS2   flags after obj file list (libraries, etc)
CFLAGS_NOOPT = -I. -DUNIX $(LOCAL_ZIP)
CFLAGS = -O2 $(CFLAGS_NOOPT)
LFLAGS1 =
LFLAGS2 = -s

# object file lists
OBJZ = zip.o zipfile.o zipup.o fileio.o util.o globals.o crypt.o ttyio.o \
       unix.o crc32.o zbz2err.o
OBJI = deflate.o trees.o
OBJA =
OCRCU8 =
OCRCTB = crc32_.o
OBJU = zipfile_.o fileio_.o util_.o globals.o unix_.o $(OCRCU8)
OBJN = zipnote.o  $(OBJU)
OBJC = zipcloak.o $(OBJU) $(OCRCTB) crypt_.o ttyio.o
OBJS = zipsplit.o $(OBJU)

ZIP_H = zip.h ziperr.h tailor.h unix/osdep.h

# suffix rules
.SUFFIXES:
.SUFFIXES: _.o .o .c .doc .1
.c_.o:
	$(CC) -c $(CFLAGS) -DUTIL -o $@ $<

.c.o:
	$(CC) -c $(CFLAGS) $<

.1.doc:
	nroff -man $< | col -bx | uniq > $@

# rules for zip, zipnote, zipcloak, zipsplit, and the Zip MANUALs.
$(OBJZ): $(ZIP_H)
$(OBJI): $(ZIP_H)
$(OBJN): $(ZIP_H)
$(OBJS): $(ZIP_H)
$(OBJC): $(ZIP_H)
zip.o zipup.o zipfile.o fileio.o crc32.o crypt.o: crc32.h
zipcloak.o zipfile_.o fileio_.o crc32_.o crypt_.o: crc32.h
zip.o zipup.o crypt.o ttyio.o zipcloak.o crypt_.o: crypt.h
zip.o zipup.o zipnote.o zipcloak.o zipsplit.o: revision.h
zip.o crypt.o ttyio.o zipcloak.o crypt_.o: ttyio.h
zipup.o: unix/zipup.h

match.o: match.S
	$(CPP) match.S > _match.s
	$(AS) _match.s
	mv _match.o match.o
	rm -f _match.s

crc_i386.o: crc_i386.S
	$(CPP) crc_i386.S > _crc_i386.s
	$(AS) _crc_i386.s
	mv _crc_i386.o crc_i386.o
	rm -f _crc_i386.s

unix.o: unix/unix.c
	$(CC) -c $(CFLAGS) unix/unix.c

unix_.o: unix/unix.c
	$(CC) -c $(CFLAGS) -DUTIL -o $@ unix/unix.c

ZIPS = zip$E zipcloak$E zipnote$E zipsplit$E

zips: $(ZIPS)
zipsman: $(ZIPS) $(ZIPMANUALs)

zip$E: $(OBJZ) $(OBJI) $(OBJA) $(LIB_BZ)
	$(BIND) -o zip$E $(LFLAGS1) $(OBJZ) $(OBJI) $(OBJA) $(LFLAGS2)
zipnote$E: $(OBJN)
	$(BIND) -o zipnote$E $(LFLAGS1) $(OBJN) $(LFLAGS2)
zipcloak$E: $(OBJC) $(OCRCTB)
	$(BIND) -o zipcloak$E $(LFLAGS1) $(OBJC) $(LFLAGS2)
zipsplit$E: $(OBJS)
	$(BIND) -o zipsplit$E $(LFLAGS1) $(OBJS) $(LFLAGS2)

$(ZIPMANUAL): man/zip.1
	nroff -man man/zip.1 | col -bx | uniq > $(ZIPMANUAL)

$(ZIPMANUALcloak): man/zipcloak.1
	nroff -man man/zipcloak.1 | col -bx | uniq > $(ZIPMANUALcloak)

$(ZIPMANUALnote): man/zipnote.1
	nroff -man man/zipnote.1 | col -bx | uniq > $(ZIPMANUALnote)

$(ZIPMANUALsplit): man/zipsplit.1
	nroff -man man/zipsplit.1 | col -bx | uniq > $(ZIPMANUALsplit)


# bzip2 object library

$(IZ_OUR_BZIP2_DIR)/libbz2.a : $(IZ_OUR_BZIP2_DIR)/Makefile
	@echo "Building bzip2 object library..."
	( cd $(IZ_OUR_BZIP2_DIR); \
	  $(MAKE) CC="$(CC_BZ)" CFLAGS="$(CFLAGS_BZ)" libbz2.a )
	@echo "   bzip2 object library created."


# install
install:        $(ZIPS)
	-$(INSTALL_D) $(BINDIR)
	$(INSTALL_PROGRAM) $(ZIPS) $(BINDIR)
	-cd $(BINDIR); $(CHMOD) $(BINFLAGS) $(ZIPS)
	-$(INSTALL_D) $(MANDIR)
	$(INSTALL_PROGRAM) man/zip.1 $(MANDIR)/zip.$(MANEXT)
	$(CHMOD) $(MANFLAGS) $(MANDIR)/zip.$(MANEXT)
	$(INSTALL_PROGRAM) man/zipcloak.1 $(MANDIR)/zipcloak.$(MANEXT)
	$(CHMOD) $(MANFLAGS) $(MANDIR)/zipcloak.$(MANEXT)
	$(INSTALL_PROGRAM) man/zipnote.1 $(MANDIR)/zipnote.$(MANEXT)
	$(CHMOD) $(MANFLAGS) $(MANDIR)/zipnote.$(MANEXT)
	$(INSTALL_PROGRAM) man/zipsplit.1 $(MANDIR)/zipsplit.$(MANEXT)
	$(CHMOD) $(MANFLAGS) $(MANDIR)/zipsplit.$(MANEXT)

uninstall:
	-cd $(BINDIR); rm -f $(ZIPS)
	-cd $(MANDIR); rm -f \
	 zip.$(MANEXT) zipcloak.$(MANEXT) zipnote.$(MANEXT) zipsplit.$(MANEXT)


flags:  unix/configure
	sh unix/configure "${CC}" "${CFLAGS_NOOPT}" "${IZ_BZIP2}"

# These symbols, when #defined using -D have these effects on compilation:
# ZMEM                  - includes C language versions of memset(), memcpy(),
#                         and memcmp() (util.c).
# HAVE_DIRENT_H         - use <dirent.h> instead of <sys/dir.h>
# NODIR                 - for 3B1, which has neither getdents() nor opendir().
# HAVE_NDIR_H           - use <ndir.h> (unix/unix.c).
# HAVE_SYS_DIR_H        - use <sys/dir.h>
# HAVE_SYS_NDIR_H       - use <sys/ndir.h>
# UTIL                  - select routines for utilities (note, cloak, split)
# NO_RMDIR              - remove directories using a system("rmdir ...") call.
# NO_PROTO              - cannot handle ANSI prototypes
# NO_CONST              - cannot handle ANSI const
# NO_LARGE_FILE_SUPPORT - do not enable Large File support even if available.
# NO_ZIP64_SUPPORT      - do not enable Zip64 archive support even if available.
# NO_UNICODE_SUPPORT    - do not enable Unicode support even if available.
# NO_BZIP2_SUPPORT      - do not compile in bzip2 code even if available.

#               Generic targets:

generic: flags
	eval $(MAKE) $(MAKEF) zips `cat flags`

generic_gcc:
	$(MAKE) $(MAKEF) generic CC=gcc CPP="gcc -E"

# AT&T 6300 PLUS (don't know yet how to allocate 64K bytes):
att6300nodir:
	$(MAKE) $(MAKEF) zips LFLAGS1="-Ml -s" \
	CFLAGS="-DUNIX -I. -O -Ml -DNO_RMDIR -DDYN_ALLOC -DMEDIUM_MEM \
-DWSIZE=16384 -DNO_STDLIB_H -DNO_STDDEF_H -DNO_RENAME \
-DNO_MKTIME -DNO_SIZE_T -DNO_VOID -DNO_PROTO -DNO_DIR \
-DNO_CONST -DHAVE_TERMIO_H" \
	"LFLAGS2="

# Coherent (AS definition not needed for gcc)
coherent:
	$(MAKE) $(MAKEF) zips CFLAGS="-DUNIX -I. -O -DDIRENT -DASMV" \
	 AS="as -gx" OBJA=match.o

# Cray Unicos 6.1, Standard C compiler 3.0 (all routines except trees.c
# may be compiled with vector3; internal compiler bug in 3.0.2.3 and
# earlier requires vector2 for trees.c)
cray_v3:
	$(MAKE) $(MAKEF) zips CC="scc" \
		CFLAGS="-DUNIX -I. -O -h vector2 -h scalar3 -DHAVE_DIRENT_H"

# Cygwin
cygwin:
	$(MAKE) $(MAKEF) generic CC="gcc" CPP="gcc -E" EXE=".exe"

# LynxOS
lynx:
	$(MAKE) $(MAKEF) generic CC=gcc CPP="gcc -E" CFLAGS="$(CFLAGS) \
	 -DNO_UNDERLINE -DLynx -DLYNX LFLAGS2="$LFLAGS2 -lc_p"

# MINIX 1.5.10 with Bruce Evans 386 patches and gcc/GNU make
minix:
	$(MAKE) $(MAKEF) zips CFLAGS="-DUNIX -I. -O -DDIRENT -DMINIX" CC=gcc
	chmem =262144 zip

# IBM OS/390 (formerly MVS) compiled under "OpenEdition" shell
# You can make the zip executable with IBM's make, but you will
# get errors dealing with the _.o targets for the other executables
# (zipcloak, etc).  GNU make will build all the executables.
# If you have GNU make in your path as gmake, you can uncomment
# the following, but it shouldn't be needed:
#MAKE = gmake

os390:
	$(MAKE) $(MAKEF) zips CFLAGS="$(CF) -I. -DUNIX -DOS390 -DEBCDIC \
	 -DSYSV -DNO_PARAM_H" LFLAGS2=""

# QNX is "special" because out /bin/sh is ksh and it doesn't grok the
# configure script properly, generating a bad flags file.  D'oh! [cjh]
#
# QNX/Neutrino is "special" because you don't have any native development
# tools yet.  Set ARCH to "x86", "ppcbe", "ppcle", "mipsbe", or "mipsle"
# to produce x86, PowerPC (big- or little-endian) and MIPS (big-
# or little-endian) using gcc. [cjh]
qnx:
	$(MAKE) $(MAKEF) zips LN=ln CC=cc CFLAGS="-DUNIX -I. -O \
	 -DHAVE_DIRENT_H -DHAVE_TERMIOS_H -DNO_MKTEMP"

qnxnto:
	@if [ "$(ARCH)" = "" ] ; then \
		echo "You didn't set ARCH; I'll assume you meant ARCH=x86..." ; \
		echo "" ; \
		$(MAKE) $(MAKEF) zips LN=ln CC="qcc -Vgcc_ntox86" \
			CFLAGS="-g -DUNIX -I. -O -DHAVE_DIRENT_H -DHAVE_TERMIOS_H -DNO_MKTEMP" \
			LFLAGS2=-g ; \
	else \
		echo "Making zip for $(ARCH)..." ; \
		echo "" ; \
		$(MAKE) $(MAKEF) zips LN=ln CC="qcc -Vgcc_nto$(ARCH)" \
			CFLAGS="-g -DUNIX -I. -O -DHAVE_DIRENT_H -DHAVE_TERMIOS_H -DNO_MKTEMP" \
			LFLAGS2=-g ; \
	fi

# Solaris:  Generic, plus generation of installable package.
solaris:	generic svr4package

# Solaris with GCC: generic_gcc, plus generation of installable package
solaris_gcc:	generic_gcc svr4package

# Package generation interface (by JBush). Originally tested under Sun Solaris.
# Other SVr4s may be very similar, and could possibly use this.
# Note:  Expects version info to be stored in VERSION macro variable.
# See "README" under ./unix/Packaging
svr4package:
	@echo "Creating SVR4 package for Unix ..."
	-@rm -rf ./$(PKGDIR) ./$(PKGDIR)_`uname -p`.pkg
	-@sed -e "s/.VERSION./$(VERSION)/g" \
	      -e "s/.PSTAMP./$(LOGNAME)_`date | tr  ' ' '_'`/g" \
	      -e "s/.ARCH./Solaris_`uname -rp | tr ' ' ','`/g" \
	      ./unix/Packaging/pkginfo.in > ./unix/Packaging/pkginfo
	-@sed -e "s/.ARCH./`uname -p`/g" \
	      ./unix/Packaging/preinstall.in > ./unix/Packaging/preinstall
	/usr/bin/pkgmk -d . -b . -r . -f ./unix/Packaging/prototype $(PKGDIR)
	/usr/bin/pkgtrans -o -s . $(PKGDIR)_`uname -p`.pkg $(PKGDIR)
	@echo " "
	@echo "To install, copy $(PKGDIR)_`uname -p`.pkg to the target system, and"
	@echo "issue the command (as root):  pkgadd -d $(PKGDIR)_`uname -p`.pkg"
	@echo " "

# make a distribution
dist:	$(ZIPMANUAL)
	eval zip -r9 zip`sed -e '/VERSION/!d' -e 's/.*"\(.*\)".*/\1/' \
			  -e 's/[.]//g' -e 's/ .*//g' -e q revision.h` *

# clean up after making stuff and installing it
clean:
	rm -f *.o $(ZIPS) flags
	rm -rf $(PKGDIR)

clean_bzip2 :
	@if test -f "$(IZ_OUR_BZIP2_DIR)/Makefile"; then \
	  ( cd $(IZ_OUR_BZIP2_DIR); make clean ); \
	else \
          if test -z "$(IZ_OUR_BZIP2_DIR)"; then \
	    echo "No bzip2 directory (\"IZ_OUR_BZIP2_DIR\") specified."; \
	  else \
	    echo "No bzip2 make file found: $(IZ_OUR_BZIP2_DIR)/Makefile."; \
	  fi; \
	fi

clean_exe :
	rm -f $(ZIPS)
#
