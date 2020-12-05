#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘
# Description:
#   Cosmopolitan Testing Library.

PKGS += LIBC_TESTLIB

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_ARTIFACTS += LIBC_TESTLIB_A
LIBC_TESTLIB = $(LIBC_TESTLIB_A_DEPS) $(LIBC_TESTLIB_A)
LIBC_TESTLIB_A = o/$(MODE)/libc/testlib/testlib.a
LIBC_TESTLIB_A_CHECKS = $(LIBC_TESTLIB_A).pkg

LIBC_TESTLIB_A_ASSETS =						\
	libc/testlib/hyperion.txt

LIBC_TESTLIB_A_HDRS =						\
	libc/testlib/bench.h					\
	libc/testlib/ezbench.h					\
	libc/testlib/hyperion.h					\
	libc/testlib/testlib.h

LIBC_TESTLIB_A_SRCS_S =						\
	libc/testlib/bench.S					\
	libc/testlib/combo.S					\
	libc/testlib/fixture.S					\
	libc/testlib/hyperion.S					\
	libc/testlib/testcase.S					\
	libc/testlib/thrashcodecache.S				\
	libc/testlib/thunks/assert_eq.S				\
	libc/testlib/thunks/assert_false.S			\
	libc/testlib/thunks/assert_ne.S				\
	libc/testlib/thunks/assert_true.S			\
	libc/testlib/thunks/expect_eq.S				\
	libc/testlib/thunks/expect_false.S			\
	libc/testlib/thunks/expect_ne.S				\
	libc/testlib/thunks/expect_true.S			\
	libc/testlib/thunks/free.S				\
	libc/testlib/thunks/jump.S

LIBC_TESTLIB_A_SRCS_C =						\
	libc/testlib/almostequallongdouble.c			\
	libc/testlib/hexequals.c				\
	libc/testlib/binequals.c				\
	libc/testlib/clearxmmregisters.c			\
	libc/testlib/formatbool.c				\
	libc/testlib/formatrange.c				\
	libc/testlib/globals.c					\
	libc/testlib/incrementfailed.c				\
	libc/testlib/formatfloat.c				\
	libc/testlib/formatbinaryasglyphs.c			\
	libc/testlib/formatbinaryashex.c			\
	libc/testlib/formatint.c				\
	libc/testlib/formatstr.c				\
	libc/testlib/shoulddebugbreak.c				\
	libc/testlib/showerror.c				\
	libc/testlib/showerror_.c				\
	libc/testlib/testmem.c					\
	libc/testlib/strequals.c				\
	libc/testlib/startswith.c				\
	libc/testlib/endswith.c					\
	libc/testlib/contains.c					\
	libc/testlib/strcaseequals.c				\
	libc/testlib/benchrunner.c				\
	libc/testlib/testrunner.c				\
	libc/testlib/comborunner.c				\
	libc/testlib/fixturerunner.c				\
	libc/testlib/ezbenchreport.c				\
	libc/testlib/ezbenchcontrol.c

LIBC_TESTLIB_A_SRCS =						\
	$(LIBC_TESTLIB_A_SRCS_S)				\
	$(LIBC_TESTLIB_A_SRCS_C)

LIBC_TESTLIB_A_OBJS =						\
	$(LIBC_TESTLIB_A_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(LIBC_TESTLIB_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_TESTLIB_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_TESTLIB_A_ASSETS:%=o/$(MODE)/%.zip.o)

LIBC_TESTLIB_A_DIRECTDEPS =					\
	APE_LIB							\
	LIBC_ALG						\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_KERNEL32					\
	LIBC_RAND						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_TIME						\
	LIBC_TINYMATH						\
	LIBC_STUBS						\
	LIBC_SYSV_CALLS						\
	LIBC_UNICODE						\
	LIBC_X							\
	LIBC_ZIPOS						\
	THIRD_PARTY_GDTOA

LIBC_TESTLIB_A_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_TESTLIB_A_DIRECTDEPS),$($(x))))

$(LIBC_TESTLIB_A):						\
		libc/testlib/					\
		$(LIBC_TESTLIB_A).pkg				\
		$(LIBC_TESTLIB_A_OBJS)

$(LIBC_TESTLIB_A).pkg:						\
		$(LIBC_TESTLIB_A_OBJS)				\
		$(foreach x,$(LIBC_TESTLIB_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_ARTIFACTS += LIBC_TESTLIB_RUNNER_A
LIBC_TESTLIB_RUNNER = $(LIBC_TESTLIB_RUNNER_A_DEPS) $(LIBC_TESTLIB_RUNNER_A)
LIBC_TESTLIB_RUNNER_A = o/$(MODE)/libc/testlib/runner.a
LIBC_TESTLIB_RUNNER_A_SRCS = libc/testlib/runner.c
LIBC_TESTLIB_RUNNER_A_CHECKS = $(LIBC_TESTLIB_RUNNER_A).pkg

LIBC_TESTLIB_RUNNER_A_OBJS =					\
	$(LIBC_TESTLIB_RUNNER_A_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(LIBC_TESTLIB_RUNNER_A_SRCS:%.c=o/$(MODE)/%.o)

LIBC_TESTLIB_RUNNER_A_DIRECTDEPS =				\
	LIBC_FMT						\
	LIBC_RUNTIME						\
	LIBC_NEXGEN32E						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_TESTLIB

LIBC_TESTLIB_RUNNER_A_DEPS :=					\
	$(call uniq,$(foreach x,$(LIBC_TESTLIB_RUNNER_A_DIRECTDEPS),$($(x))))

$(LIBC_TESTLIB_RUNNER_A):					\
		libc/testlib/					\
		$(LIBC_TESTLIB_RUNNER_A).pkg			\
		$(LIBC_TESTLIB_RUNNER_A_OBJS)

$(LIBC_TESTLIB_RUNNER_A).pkg:					\
		$(LIBC_TESTLIB_RUNNER_A_OBJS)			\
		$(foreach x,$(LIBC_TESTLIB_RUNNER_A_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_ARTIFACTS += LIBC_TESTMAIN

LIBC_TESTMAIN =							\
	$(LIBC_TESTMAIN_DEPS)					\
	$(LIBC_TESTMAIN_OBJS)

LIBC_TESTMAIN_CHECKS =						\
	o/$(MODE)/libc/testlib/testmain.pkg

LIBC_TESTMAIN_SRCS =						\
	libc/testlib/testmain.c

LIBC_TESTMAIN_OBJS =						\
	$(LIBC_TESTMAIN_SRCS:%=o/$(MODE)/%.zip.o)		\
	o/$(MODE)/libc/testlib/testmain.o

LIBC_TESTMAIN_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_LOG						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TESTLIB						\
	LIBC_TESTLIB_RUNNER					\
	THIRD_PARTY_GETOPT

LIBC_TESTMAIN_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_TESTMAIN_DIRECTDEPS),$($(x))))

o/$(MODE)/libc/testlib/testmain.pkg:				\
		$(LIBC_TESTMAIN_OBJS)				\
		$(foreach x,$(LIBC_TESTMAIN_DIRECTDEPS),$($(x)_A).pkg)

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_LIBS = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_A))
LIBC_TESTLIB_ARCHIVES = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_A))
LIBC_TESTLIB_SRCS = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_SRCS))
LIBC_TESTLIB_HDRS = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_HDRS))
LIBC_TESTLIB_CHECKS = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_CHECKS))
LIBC_TESTLIB_OBJS = $(foreach x,$(LIBC_TESTLIB_ARTIFACTS),$($(x)_OBJS))
$(LIBC_TESTLIB_OBJS): $(BUILD_FILES) libc/libc.mk

.PHONY: o/$(MODE)/libc/testlib
o/$(MODE)/libc/testlib: $(LIBC_TESTLIB_LIBS) $(LIBC_TESTLIB_CHECKS)
