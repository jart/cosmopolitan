#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TOOL_PLINKO

TOOL_PLINKO_ARTIFACTS += TOOL_PLINKO_A
TOOL_PLINKO = $(TOOL_PLINKO_A_DEPS) $(TOOL_PLINKO_A)
TOOL_PLINKO_A = o/$(MODE)/tool/plinko/plinkolib.a
TOOL_PLINKO_A_FILES := $(filter-out %/.%,$(wildcard tool/plinko/*))
TOOL_PLINKO_A_HDRS = $(filter %.h,$(TOOL_PLINKO_A_FILES))
TOOL_PLINKO_A_SRCS = $(filter-out tool/plinko/main.c,$(filter %.c,$(TOOL_PLINKO_A_FILES)))
TOOL_PLINKO_A_OBJS = $(TOOL_PLINKO_A_SRCS:%.c=o/$(MODE)/%.o)

TOOL_PLINKO_A_CHECKS =				\
	$(TOOL_PLINKO_A_HDRS:%=o/$(MODE)/%.ok)	\
	$(TOOL_PLINKO_A).pkg

TOOL_PLINKO_A_DIRECTDEPS =			\
	LIBC_CALLS				\
	LIBC_FMT				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_RUNTIME				\
	LIBC_STDIO				\
	LIBC_STR				\
	LIBC_SYSV				\
	LIBC_SYSV_CALLS				\
	THIRD_PARTY_COMPILER_RT			\
	THIRD_PARTY_GETOPT			\
	THIRD_PARTY_MUSL			\

TOOL_PLINKO_A_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_PLINKO_A_DIRECTDEPS),$($(x))))

$(TOOL_PLINKO_A):				\
		$(TOOL_PLINKO_A).pkg		\
		$(TOOL_PLINKO_A_OBJS)

$(TOOL_PLINKO_A).pkg:				\
		$(TOOL_PLINKO_A_OBJS)		\
		$(foreach x,$(TOOL_PLINKO_A_DIRECTDEPS),$($(x)_A).pkg)

ifeq ($(MODE),dbg)
$(TOOL_PLINKO_A_OBJS): private CFLAGS += -fno-inline
else
ifeq ($(MODE),aarch64-dbg)
$(TOOL_PLINKO_A_OBJS): private CFLAGS += -fno-inline
else
$(TOOL_PLINKO_A_OBJS): private CFLAGS += -foptimize-sibling-calls
endif
endif

$(TOOL_PLINKO_A_OBJS): tool/plinko/BUILD.mk

################################################################################

TOOL_PLINKO_ARTIFACTS += TOOL_PLINKO_MAIN
TOOL_PLINKO_MAIN_SRCS = tool/plinko/main.c
TOOL_PLINKO_MAIN_OBJS = $(TOOL_PLINKO_MAIN_SRCS:%.c=o/$(MODE)/%.o)
TOOL_PLINKO_MAIN_COMS = o/$(MODE)/tool/plinko/plinko

TOOL_PLINKO_MAIN_BINS =					\
	$(TOOL_PLINKO_MAIN_COMS)			\
	$(TOOL_PLINKO_MAIN_COMS:%=%.dbg)

TOOL_PLINKO_MAIN_DIRECTDEPS =				\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	TOOL_PLINKO					\

TOOL_PLINKO_MAIN_DEPS :=				\
	$(call uniq,$(foreach x,$(TOOL_PLINKO_MAIN_DIRECTDEPS),$($(x))))

o/$(MODE)/tool/plinko/plinko.pkg:			\
		$(TOOL_PLINKO_MAIN_OBJS)		\
		$(foreach x,$(TOOL_PLINKO_MAIN_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/tool/plinko/plinko.dbg:			\
		$(TOOL_PLINKO_MAIN_DEPS)		\
		$(TOOL_PLINKO_MAIN_OBJS)		\
		o/$(MODE)/tool/plinko/plinko.pkg	\
		o/$(MODE)/tool/plinko/library.lisp.zip.o	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TOOL_PLINKO_MAIN_OBJS): tool/plinko/BUILD.mk

o/$(MODE)/tool/plinko/plinko.zip.o:			\
		o/$(MODE)/tool/plinko/plinko

o/$(MODE)/tool/plinko/plinko.zip.o			\
o/$(MODE)/tool/plinko/library.lisp.zip.o		\
o/$(MODE)/tool/plinko/binarytrees.lisp.zip.o		\
o/$(MODE)/tool/plinko/algebra.lisp.zip.o		\
o/$(MODE)/tool/plinko/infix.lisp.zip.o			\
o/$(MODE)/tool/plinko/kanren.lisp.zip.o			\
o/$(MODE)/tool/plinko/ok.lisp.zip.o:			\
		ZIPOBJ_FLAGS +=				\
			-B

################################################################################

TOOL_PLINKO_SRCS = $(foreach x,$(TOOL_PLINKO_ARTIFACTS),$($(x)_SRCS))
TOOL_PLINKO_HDRS = $(foreach x,$(TOOL_PLINKO_ARTIFACTS),$($(x)_HDRS))
TOOL_PLINKO_BINS = $(foreach x,$(TOOL_PLINKO_ARTIFACTS),$($(x)_BINS))
TOOL_PLINKO_OBJS = $(foreach x,$(TOOL_PLINKO_ARTIFACTS),$($(x)_OBJS))
TOOL_PLINKO_CHECKS = $(foreach x,$(TOOL_PLINKO_ARTIFACTS),$($(x)_CHECKS))

.PHONY: o/$(MODE)/tool/plinko
o/$(MODE)/tool/plinko: $(TOOL_PLINKO_CHECKS) $(TOOL_PLINKO_BINS)
