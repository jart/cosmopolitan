#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_ZIP

THIRD_PARTY_ZIP_FILES :=				\
	$(wildcard third_party/infozip/zip/*)		\
	$(wildcard third_party/infozip/zip/unix/*)

THIRD_PARTY_ZIP_SRCS = $(filter %.c,$(THIRD_PARTY_ZIP_FILES))
THIRD_PARTY_ZIP_HDRS = $(filter %.h,$(THIRD_PARTY_ZIP_FILES))
THIRD_PARTY_ZIP_INCS = $(filter %.inc,$(THIRD_PARTY_ZIP_FILES))

THIRD_PARTY_ZIP_COMS = 					\
	o/$(MODE)/third_party/infozip/zip.com 		\
	o/$(MODE)/third_party/infozip/zipsplit.com 	\
	o/$(MODE)/third_party/infozip/zipnote.com 	\
	o/$(MODE)/third_party/infozip/zipcloak.com

THIRD_PARTY_ZIP_BINS = 					\
	$(THIRD_PARTY_ZIP_COMS)				\
	$(THIRD_PARTY_ZIP_COMS:%=%.dbg)

THIRD_PARTY_ZIP_OBJS = $(sort				\
	$(THIRD_PARTY_ZIP_COM_OBJS)			\
	$(THIRD_PARTY_ZIPCLOAK_OBJS)			\
	$(THIRD_PARTY_ZIPNOTE_OBJS)			\
	$(THIRD_PARTY_ZIPSPLIT_OBJS)			\
	)

THIRD_PARTY_ZIP_UTIL_OBJS1 =				\
	o/$(MODE)/third_party/infozip/zip/globals.o 	\
	o/$(MODE)/third_party/infozip/zip/unix/unix_.o 	\
	o/$(MODE)/third_party/infozip/zip/zipfile_.o 	\
	o/$(MODE)/third_party/infozip/zip/fileio_.o 	\
	o/$(MODE)/third_party/infozip/zip/util_.o

THIRD_PARTY_ZIP_UTIL_OBJS2 =				\
	o/$(MODE)/third_party/infozip/zip/crypt_.o 	\
	o/$(MODE)/third_party/infozip/zip/crc32_.o

THIRD_PARTY_ZIP_UTIL_OBJS =				\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)			\
	$(THIRD_PARTY_ZIP_UTIL_OBJS2)

THIRD_PARTY_ZIP_COM_OBJS =				\
	o/$(MODE)/third_party/infozip/zip/zip.o 	\
	o/$(MODE)/third_party/infozip/zip/zipfile.o 	\
	o/$(MODE)/third_party/infozip/zip/zipup.o 	\
	o/$(MODE)/third_party/infozip/zip/fileio.o 	\
	o/$(MODE)/third_party/infozip/zip/util.o 	\
	o/$(MODE)/third_party/infozip/zip/globals.o 	\
	o/$(MODE)/third_party/infozip/zip/crypt.o 	\
	o/$(MODE)/third_party/infozip/zip/ttyio.o 	\
	o/$(MODE)/third_party/infozip/zip/unix/unix.o 	\
	o/$(MODE)/third_party/infozip/zip/crc32.o 	\
	o/$(MODE)/third_party/infozip/zip/zbz2err.o 	\
	o/$(MODE)/third_party/infozip/zip/deflate.o 	\
	o/$(MODE)/third_party/infozip/zip/trees.o

THIRD_PARTY_ZIPSPLIT_OBJS =				\
	o/$(MODE)/third_party/infozip/zip/zipsplit.o 	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)

THIRD_PARTY_ZIPNOTE_OBJS =				\
	o/$(MODE)/third_party/infozip/zip/zipnote.o 	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)

THIRD_PARTY_ZIPCLOAK_OBJS =				\
	o/$(MODE)/third_party/infozip/zip/zipcloak.o 	\
	o/$(MODE)/third_party/infozip/zip/ttyio.o 	\
	$(THIRD_PARTY_ZIP_UTIL_OBJS1)			\
	$(THIRD_PARTY_ZIP_UTIL_OBJS2)

THIRD_PARTY_ZIP_LARGE_OBJS =				\
	o/$(MODE)/third_party/infozip/zip/zip.o		\
	o/$(MODE)/third_party/infozip/zip/zipsplit.o	\
	o/$(MODE)/third_party/infozip/zip/fileio.o	\
	o/$(MODE)/third_party/infozip/zip/fileio_.o

o/$(MODE)/third_party/infozip/zip/%_.o:			\
		third_party/infozip/zip/%.c		\
		o/$(MODE)/third_party/infozip/zip/%.o
	@$(COMPILE) -AOBJECTIFY.c $(OBJECTIFY.c) $(OUTPUT_OPTION) -DUTIL $<

$(THIRD_PARTY_ZIP_OBJS):				\
		OVERRIDE_CPPFLAGS +=			\
			-DUNIX				\
			-DMMAP				\
			-DUNICODE_SUPPORT		\
			-DUSE_EF_UT_TIME		\
			-DLARGE_FILE_SUPPORT		\
			-DHAVE_DIRENT_H			\
			-DHAVE_TERMIOS_H		\
			-DNO_BZIP2_SUPPORT		\
			-DZIP64_SUPPORT

$(THIRD_PARTY_ZIP_LARGE_OBJS):				\
		OVERRIDE_CPPFLAGS +=			\
			-DSTACK_FRAME_UNLIMITED

THIRD_PARTY_ZIP_DIRECTDEPS =				\
	LIBC_ERRNO					\
	LIBC_LIMITS					\
	LIBC_ALG					\
	LIBC_FMT					\
	LIBC_STR					\
	LIBC_MEM					\
	LIBC_LOG					\
	LIBC_CALLS					\
	LIBC_STDIO					\
	LIBC_TIME					\
	LIBC_UNICODE

THIRD_PARTY_ZIP_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_ZIP_DIRECTDEPS),$($(x))))

o/$(MODE)/third_party/infozip/zip.com.dbg:		\
		$(THIRD_PARTY_ZIP_DEPS)			\
		$(THIRD_PARTY_ZIP_COM_OBJS)		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

o/$(MODE)/third_party/infozip/zipsplit.com.dbg:		\
		$(THIRD_PARTY_ZIP_DEPS)			\
		$(THIRD_PARTY_ZIPSPLIT_OBJS)		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

o/$(MODE)/third_party/infozip/zipnote.com.dbg:		\
		$(THIRD_PARTY_ZIP_DEPS)			\
		$(THIRD_PARTY_ZIPNOTE_OBJS)		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

o/$(MODE)/third_party/infozip/zipcloak.com.dbg:		\
		$(THIRD_PARTY_ZIP_DEPS)			\
		$(THIRD_PARTY_ZIPCLOAK_OBJS)		\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/third_party/infozip
o/$(MODE)/third_party/infozip:				\
		$(THIRD_PARTY_ZIP_BINS)
