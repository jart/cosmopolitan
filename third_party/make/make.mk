#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += THIRD_PARTY_MAKE

THIRD_PARTY_MAKE_COMS = \
	o/$(MODE)/third_party/make/make.com

THIRD_PARTY_MAKE_BINS = \
	$(THIRD_PARTY_MAKE_COMS) \
	$(THIRD_PARTY_MAKE_COMS:%=%.dbg)

THIRD_PARTY_MAKE_A = \
	o/$(MODE)/third_party/make/make.a

THIRD_PARTY_MAKE_CHECKS =				\
	$(THIRD_PARTY_MAKE_A).pkg

# libgnu.a recipe
THIRD_PARTY_MAKE_SRCS_LIB = \
	third_party/make/lib/basename-lgpl.c \
	third_party/make/lib/concat-filename.c \
	third_party/make/lib/dirname-lgpl.c \
	third_party/make/lib/dup2.c \
	third_party/make/lib/error.c \
	third_party/make/lib/exitfail.c \
	third_party/make/lib/fcntl.c \
	third_party/make/lib/fd-hook.c \
	third_party/make/lib/findprog-in.c \
	third_party/make/lib/getloadavg.c  \
	third_party/make/lib/getprogname.c \
	third_party/make/lib/stripslash.c \
	third_party/make/lib/unistd.c \
	third_party/make/lib/xalloc-die.c \
	third_party/make/lib/xconcat-filename.c \
	third_party/make/lib/xmalloc.c

THIRD_PARTY_MAKE_SRCS_BASE = \
	third_party/make/src/ar.c \
	third_party/make/src/arscan.c \
	third_party/make/src/commands.c \
	third_party/make/src/default.c \
	third_party/make/src/dir.c \
	third_party/make/src/expand.c \
	third_party/make/src/file.c \
	third_party/make/src/function.c \
	third_party/make/src/getopt.c \
	third_party/make/src/getopt1.c \
	third_party/make/src/guile.c \
	third_party/make/src/hash.c \
	third_party/make/src/implicit.c \
	third_party/make/src/job.c \
	third_party/make/src/load.c \
	third_party/make/src/loadapi.c \
	third_party/make/src/main.c \
	third_party/make/src/misc.c \
	third_party/make/src/output.c \
	third_party/make/src/posixos.c \
	third_party/make/src/read.c \
	third_party/make/src/remake.c \
	third_party/make/src/remote-stub.c \
	third_party/make/src/rule.c \
	third_party/make/src/signame.c \
	third_party/make/src/strcache.c \
	third_party/make/src/variable.c \
	third_party/make/src/version.c \
	third_party/make/src/vpath.c

THIRD_PARTY_MAKE_SRCS = \
	$(THIRD_PARTY_MAKE_SRCS_BASE) \
	$(THIRD_PARTY_MAKE_SRCS_LIB)

THIRD_PARTY_MAKE_OBJS = \
	$(THIRD_PARTY_MAKE_SRCS:%.c=o/$(MODE)/%.o)

THIRD_PARTY_MAKE_DIRECTDEPS =	\
	LIBC_CALLS					\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_SYSV					\
	LIBC_SYSV_CALLS				\
	LIBC_TIME					\
	LIBC_X						\
	LIBC_TINYMATH				\
	LIBC_UNICODE				\
	THIRD_PARTY_COMPILER_RT		\
	THIRD_PARTY_MUSL			\
	THIRD_PARTY_GDTOA

THIRD_PARTY_MAKE_DEPS :=					\
	$(call uniq,$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_MAKE_A).pkg:				\
	$(THIRD_PARTY_MAKE_OBJS)			\
	$(foreach x,$(THIRD_PARTY_MAKE_DIRECTDEPS),$($(x)_A).pkg)

$(THIRD_PARTY_MAKE_A):					\
	third_party/make/			\
	$(THIRD_PARTY_MAKE_A).pkg		\
	$(filter-out %main.o,$(THIRD_PARTY_MAKE_OBJS))

o/$(MODE)/third_party/make/make.com.dbg:			\
		$(THIRD_PARTY_MAKE_DEPS)			\
		$(THIRD_PARTY_MAKE_A)			\
		$(THIRD_PARTY_MAKE_A).pkg		\
		o/$(MODE)/third_party/make/src/main.o	\
		$(CRT)					\
		$(APE)
	@$(APELINK)


$(THIRD_PARTY_MAKE_OBJS):				\
		OVERRIDE_CFLAGS +=			\
			-isystem libc/isystem	\
			-DHAVE_CONFIG_H			\
			-DINCLUDEDIR=\".\"		\
			-DLIBDIR=\".\"		\
			-DLOCALEDIR=\".\"

.PHONY: o/$(MODE)/third_party/make
o/$(MODE)/third_party/make:				\
		$(THIRD_PARTY_MAKE_BINS)			\
		$(THIRD_PARTY_MAKE_CHECKS)
