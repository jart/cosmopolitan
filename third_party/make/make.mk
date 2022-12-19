#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_MAKE

THIRD_PARTY_MAKE_COMS =					\
	o/$(MODE)/third_party/make/make.com

THIRD_PARTY_MAKE_BINS =					\
	$(THIRD_PARTY_MAKE_COMS)			\
	$(THIRD_PARTY_MAKE_COMS:%=%.dbg)

THIRD_PARTY_MAKE_A =					\
	o/$(MODE)/third_party/make/make.a

THIRD_PARTY_MAKE_HDRS =					\
	third_party/make/filename.h			\
	third_party/make/dirname.h			\
	third_party/make/stddef.h			\
	third_party/make/error.h			\
	third_party/make/gnumake.h			\
	third_party/make/gettext.h			\
	third_party/make/stdlib.h			\
	third_party/make/xalloc.h			\
	third_party/make/xalloc-oversized.h		\
	third_party/make/os.h				\
	third_party/make/stdint.h			\
	third_party/make/fd-hook.h			\
	third_party/make/job.h				\
	third_party/make/unistd.h			\
	third_party/make/getprogname.h			\
	third_party/make/dosname.h			\
	third_party/make/config.h			\
	third_party/make/concat-filename.h		\
	third_party/make/findprog.h			\
	third_party/make/intprops.h			\
	third_party/make/exitfail.h			\
	third_party/make/alloca.h			\
	third_party/make/hash.h				\
	third_party/make/rule.h				\
	third_party/make/filedef.h			\
	third_party/make/fcntl.h			\
	third_party/make/stdio.h			\
	third_party/make/variable.h			\
	third_party/make/debug.h			\
	third_party/make/output.h			\
	third_party/make/getopt.h			\
	third_party/make/dep.h				\
	third_party/make/commands.h


THIRD_PARTY_MAKE_INCS =					\
	third_party/make/makeint.inc

THIRD_PARTY_MAKE_CHECKS =				\
	$(THIRD_PARTY_MAKE_A).pkg

# libgnu.a recipe
THIRD_PARTY_MAKE_SRCS_LIB =				\
	third_party/make/basename-lgpl.c		\
	third_party/make/concat-filename.c		\
	third_party/make/dirname-lgpl.c			\
	third_party/make/error.c			\
	third_party/make/exitfail.c			\
	third_party/make/fcntl.c			\
	third_party/make/fd-hook.c			\
	third_party/make/findprog-in.c			\
	third_party/make/getprogname.c			\
	third_party/make/stripslash.c			\
	third_party/make/unistd.c			\
	third_party/make/xalloc-die.c			\
	third_party/make/xconcat-filename.c		\
	third_party/make/xmalloc.c

THIRD_PARTY_MAKE_SRCS_BASE =				\
	third_party/make/commands.c			\
	third_party/make/default.c			\
	third_party/make/dir.c				\
	third_party/make/expand.c			\
	third_party/make/file.c				\
	third_party/make/function.c			\
	third_party/make/getopt.c			\
	third_party/make/getopt1.c			\
	third_party/make/guile.c			\
	third_party/make/hash.c				\
	third_party/make/implicit.c			\
	third_party/make/job.c				\
	third_party/make/load.c				\
	third_party/make/loadapi.c			\
	third_party/make/main.c				\
	third_party/make/misc.c				\
	third_party/make/output.c			\
	third_party/make/posixos.c			\
	third_party/make/read.c				\
	third_party/make/remake.c			\
	third_party/make/remote-stub.c			\
	third_party/make/rule.c				\
	third_party/make/strcache.c			\
	third_party/make/variable.c			\
	third_party/make/version.c			\
	third_party/make/vpath.c

THIRD_PARTY_MAKE_SRCS =					\
	$(THIRD_PARTY_MAKE_SRCS_BASE)			\
	$(THIRD_PARTY_MAKE_SRCS_LIB)

THIRD_PARTY_MAKE_OBJS =					\
	$(THIRD_PARTY_MAKE_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MAKE_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_ELF					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SOCK					\
	LIBC_NT_KERNEL32				\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS					\
	LIBC_TIME					\
	LIBC_TINYMATH					\
	LIBC_X						\
	THIRD_PARTY_COMPILER_RT				\
	THIRD_PARTY_MUSL				\
	THIRD_PARTY_GDTOA

THIRD_PARTY_MAKE_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MAKE_A).pkg:				\
	$(THIRD_PARTY_MAKE_OBJS)			\
	$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MAKE_A):					\
	third_party/make/				\
	$(THIRD_PARTY_MAKE_A).pkg			\
	$(filter-out %main.o,$(THIRD_PARTY_MAKE_OBJS))

o/$(MODE)/third_party/make/make.com.dbg:		\
		$(THIRD_PARTY_MAKE_DEPS)		\
		$(THIRD_PARTY_MAKE_A)			\
		$(THIRD_PARTY_MAKE_A).pkg		\
		o/$(MODE)/third_party/make/main.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

o/$(MODE)/third_party/make/make.com:			\
		o/$(MODE)/third_party/make/make.com.dbg	\
		o/$(MODE)/third_party/zip/zip.com	\
		o/$(MODE)/tool/build/symtab.com
	@$(MAKE_OBJCOPY)
	@$(MAKE_SYMTAB_CREATE)
	@$(MAKE_SYMTAB_ZIP)

o/$(MODE)/third_party/make/strcache.o			\
o/$(MODE)/third_party/make/expand.o			\
o/$(MODE)/third_party/make/read.o: private		\
		OVERRIDE_CFLAGS +=			\
			-O2

o/$(MODE)/third_party/make/hash.o: private		\
		OVERRIDE_CFLAGS +=			\
			-O3

$(THIRD_PARTY_MAKE_OBJS): private			\
		OVERRIDE_CFLAGS +=			\
			-DNO_ARCHIVES			\
			-DHAVE_CONFIG_H			\
			-DSTACK_FRAME_UNLIMITED		\
			-DINCLUDEDIR=\".\"		\
			-DLIBDIR=\".\"			\
			-DLOCALEDIR=\".\"

.PHONY: o/$(MODE)/third_party/make
o/$(MODE)/third_party/make:				\
		$(THIRD_PARTY_MAKE_BINS)		\
		$(THIRD_PARTY_MAKE_CHECKS)
