#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_ZIP

THIRD_PARTY_ZIP_FILES := $(wildcard third_party/zip/*)
THIRD_PARTY_ZIP_SRCS = $(filter %.c,$(THIRD_PARTY_ZIP_FILES))
THIRD_PARTY_ZIP_HDRS = $(filter %.h,$(THIRD_PARTY_ZIP_FILES))
THIRD_PARTY_ZIP_INCS = $(filter %.inc,$(THIRD_PARTY_ZIP_FILES))

THIRD_PARTY_ZIP_COMS =				\
	o/$(MODE)/third_party/zip/zip.com	\
	o/$(MODE)/third_party/zip/zipsplit.com	\
	o/$(MODE)/third_party/zip/zipnote.com	\
	o/$(MODE)/third_party/zip/zipcloak.com

THIRD_PARTY_ZIP_BINS =				\
	$(THIRD_PARTY_ZIP_COMS)			\
	$(THIRD_PARTY_ZIP_COMS:%=%.dbg)

THIRD_PARTY_ZIP_CHECKS =			\
	o/$(MODE)/third_party/zip/zip.pkg

THIRD_PARTY_ZIP_OBJS = $(sort			\
	$(THIRD_PARTY_ZIP_ZIP_OBJS)		\
	$(THIRD_PARTY_ZIPCLOAK_OBJS)		\
	$(THIRD_PARTY_ZIPNOTE_OBJS)		\
	$(THIRD_PARTY_ZIPSPLIT_OBJS)		\
	)

THIRD_PARTY_ZIP_UTIL_OBJS1 =			\
	o/$(MODE)/third_party/zip/globals.o	\
	o/$(MODE)/third_party/zip/unix_.o	\
	o/$(MODE)/third_party/zip/zipfile_.o	\
	o/$(MODE)/third_party/zip/fileio_.o	\
	o/$(MODE)/third_party/zip/util_.o

THIRD_PARTY_ZIP_UTIL_OBJS2 =			\
	o/$(MODE)/third_party/zip/crypt_.o

THIRD_PARTY_ZIP_UTIL_OBJS =			\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)		\
	$(THIRD_PARTY_ZIP_UTIL_OBJS2)

THIRD_PARTY_ZIP_ZIP_OBJS =			\
	o/$(MODE)/third_party/zip/zip.o		\
	o/$(MODE)/third_party/zip/zipfile.o	\
	o/$(MODE)/third_party/zip/zipup.o	\
	o/$(MODE)/third_party/zip/fileio.o	\
	o/$(MODE)/third_party/zip/util.o	\
	o/$(MODE)/third_party/zip/globals.o	\
	o/$(MODE)/third_party/zip/crypt.o	\
	o/$(MODE)/third_party/zip/ttyio.o	\
	o/$(MODE)/third_party/zip/unix.o	\
	o/$(MODE)/third_party/zip/crc32.o 	\
	o/$(MODE)/third_party/zip/zbz2err.o	\
	o/$(MODE)/third_party/zip/deflate.o 	\
	o/$(MODE)/third_party/zip/trees.o

THIRD_PARTY_ZIPSPLIT_OBJS =			\
	o/$(MODE)/third_party/zip/zipsplit.o	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)

THIRD_PARTY_ZIPNOTE_OBJS =			\
	o/$(MODE)/third_party/zip/zipnote.o	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)

THIRD_PARTY_ZIPCLOAK_OBJS =			\
	o/$(MODE)/third_party/zip/zipcloak.o	\
	o/$(MODE)/third_party/zip/crc32.o	\
	o/$(MODE)/third_party/zip/ttyio.o	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)		\
	$(THIRD_PARTY_ZIP_UTIL_OBJS2)

THIRD_PARTY_ZIP_LARGE_OBJS =			\
	o/$(MODE)/third_party/zip/zip.o		\
	o/$(MODE)/third_party/zip/zipsplit.o	\
	o/$(MODE)/third_party/zip/fileio.o	\
	o/$(MODE)/third_party/zip/fileio_.o

THIRD_PARTY_ZIP_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_PROC				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_TIME				\
	LIBC_X					\
	THIRD_PARTY_BZIP2			\
	THIRD_PARTY_ZLIB

THIRD_PARTY_ZIP_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZIP_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/zip/zip.pkg:		\
		$(THIRD_PARTY_ZIP_ZIP_OBJS)	\
		$(foreach x,$(THIRD_PARTY_ZIP_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/zip/zip.com.dbg:		\
		$(THIRD_PARTY_ZIP_DEPS)		\
		$(THIRD_PARTY_ZIP_ZIP_OBJS)	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/zip/zipsplit.com.dbg:	\
		$(THIRD_PARTY_ZIP_DEPS)		\
		$(THIRD_PARTY_ZIPSPLIT_OBJS)	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/zip/zipnote.com.dbg:	\
		$(THIRD_PARTY_ZIP_DEPS)		\
		$(THIRD_PARTY_ZIPNOTE_OBJS)	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/zip/zipcloak.com.dbg:	\
		$(THIRD_PARTY_ZIP_DEPS)		\
		$(THIRD_PARTY_ZIPCLOAK_OBJS)	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/zip/crypt.o		\
o/$(MODE)/third_party/zip/crypt_.o		\
o/$(MODE)/third_party/zip/fileio.o		\
o/$(MODE)/third_party/zip/fileio_.o		\
o/$(MODE)/third_party/zip/globals.o		\
o/$(MODE)/third_party/zip/timezone.o		\
o/$(MODE)/third_party/zip/ttyio.o		\
o/$(MODE)/third_party/zip/unix.o		\
o/$(MODE)/third_party/zip/unix_.o		\
o/$(MODE)/third_party/zip/util.o		\
o/$(MODE)/third_party/zip/util_.o		\
o/$(MODE)/third_party/zip/zbz2err.o		\
o/$(MODE)/third_party/zip/zip.o			\
o/$(MODE)/third_party/zip/crc32.o		\
o/$(MODE)/third_party/zip/trees.o		\
o/$(MODE)/third_party/zip/deflate.o		\
o/$(MODE)/third_party/zip/zipcloak.o		\
o/$(MODE)/third_party/zip/zipfile.o		\
o/$(MODE)/third_party/zip/zipfile_.o		\
o/$(MODE)/third_party/zip/zipnote.o		\
o/$(MODE)/third_party/zip/zipsplit.o		\
o/$(MODE)/third_party/zip/zipup.o: private	\
	CPPFLAGS +=				\
		-w				\
		-DUNIX				\
		-DMMAP				\
		-DUNICODE_SUPPORT		\
		-DUSE_EF_UT_TIME		\
		-DLARGE_FILE_SUPPORT		\
		-DHAVE_DIRENT_H			\
		-DHAVE_TERMIOS_H		\
		-DZIP64_SUPPORT			\
		-DBZIP2_SUPPORT

.PHONY: o/$(MODE)/third_party/zip
o/$(MODE)/third_party/zip:			\
		$(THIRD_PARTY_ZIP_BINS)		\
		$(THIRD_PARTY_ZIP_CHECKS)
