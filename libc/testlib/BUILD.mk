#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘
# Description:
#   Cosmopolitan Testing Library.

PKGS += LIBC_TESTLIB

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_ARTIFACTS += LIBC_TESTLIB_A
LIBC_TESTLIB = $(LIBC_TESTLIB_A_DEPS) $(LIBC_TESTLIB_A)
LIBC_TESTLIB_A = o/$(MODE)/libc/testlib/testlib.a
LIBC_TESTLIB_A_CHECKS = $(LIBC_TESTLIB_A).pkg

LIBC_TESTLIB_A_ASSETS =						\
	libc/testlib/blocktronics.txt				\
	libc/testlib/viewables.txt				\
	libc/testlib/hyperion.txt				\
	libc/testlib/moby.txt

LIBC_TESTLIB_A_HDRS =						\
	libc/testlib/aspect.internal.h				\
	libc/testlib/bench.h					\
	libc/testlib/blocktronics.h				\
	libc/testlib/ezbench.h					\
	libc/testlib/fastrandomstring.h				\
	libc/testlib/hyperion.h					\
	libc/testlib/moby.h					\
	libc/testlib/subprocess.h				\
	libc/testlib/testlib.h					\
	libc/testlib/viewables.h

LIBC_TESTLIB_A_SRCS_S =						\
	libc/testlib/bench.S					\
	libc/testlib/blocktronics.S				\
	libc/testlib/fixture.S					\
	libc/testlib/hyperion.S					\
	libc/testlib/moby.S					\
	libc/testlib/polluteregisters.S				\
	libc/testlib/testcase.S					\
	libc/testlib/viewables.S

LIBC_TESTLIB_A_SRCS_C =						\
	libc/testlib/almostequallongdouble.c			\
	libc/testlib/benchrunner.c				\
	libc/testlib/binequals.c				\
	libc/testlib/clearxmmregisters.c			\
	libc/testlib/contains.c					\
	libc/testlib/endswith.c					\
	libc/testlib/extract.c					\
	libc/testlib/ezbenchcontrol.c				\
	libc/testlib/ezbenchreport.c				\
	libc/testlib/ezbenchwarn.c				\
	libc/testlib/fixturerunner.c				\
	libc/testlib/formatbinaryasglyphs.c			\
	libc/testlib/formatbinaryashex.c			\
	libc/testlib/formatbool.c				\
	libc/testlib/formatfloat.c				\
	libc/testlib/formatint.c				\
	libc/testlib/formatrange.c				\
	libc/testlib/formatstr.c				\
	libc/testlib/getcore.c					\
	libc/testlib/geterrno.c					\
	libc/testlib/getinterrupts.c				\
	libc/testlib/globals.c					\
	libc/testlib/hexequals.c				\
	libc/testlib/incrementfailed.c				\
	libc/testlib/memoryexists.c				\
	libc/testlib/seterrno.c					\
	libc/testlib/shoulddebugbreak.c				\
	libc/testlib/showerror.c				\
	libc/testlib/startswith.c				\
	libc/testlib/strcaseequals.c				\
	libc/testlib/strequals.c				\
	libc/testlib/strerror.c					\
	libc/testlib/testrunner.c				\
	libc/testlib/thunks.c					\
	libc/testlib/tmptest.c					\
	libc/testlib/waitforexit.c				\
	libc/testlib/waitforterm.c				\
	libc/testlib/yield.c

LIBC_TESTLIB_A_SRCS =						\
	$(LIBC_TESTLIB_A_SRCS_S)				\
	$(LIBC_TESTLIB_A_SRCS_C)

LIBC_TESTLIB_A_OBJS =						\
	$(LIBC_TESTLIB_A_SRCS_C:%.c=o/$(MODE)/%.o)		\
	$(LIBC_TESTLIB_A_SRCS_S:%.S=o/$(MODE)/%.o)		\
	$(LIBC_TESTLIB_A_ASSETS:%=o/$(MODE)/%.zip.o)		\
	o/$(MODE)/libc/testlib-test.txt.zip.o

LIBC_TESTLIB_A_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_NT_KERNEL32					\
	LIBC_PROC						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TIME						\
	LIBC_TINYMATH						\
	LIBC_X							\
	THIRD_PARTY_COMPILER_RT					\
	THIRD_PARTY_DLMALLOC					\
	THIRD_PARTY_GDTOA					\
	THIRD_PARTY_XED

LIBC_TESTLIB_A_DEPS :=						\
	$(call uniq,$(foreach x,$(LIBC_TESTLIB_A_DIRECTDEPS),$($(x))))

$(LIBC_TESTLIB_A):						\
		libc/testlib/					\
		$(LIBC_TESTLIB_A).pkg				\
		$(LIBC_TESTLIB_A_OBJS)

$(LIBC_TESTLIB_A).pkg:						\
		$(LIBC_TESTLIB_A_OBJS)				\
		$(foreach x,$(LIBC_TESTLIB_A_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/libc/testlib/blocktronics.o: libc/testlib/blocktronics.txt
o/$(MODE)/libc/testlib/viewables.o: libc/testlib/viewables.txt
o/$(MODE)/libc/testlib/hyperion.o: libc/testlib/hyperion.txt
o/$(MODE)/libc/testlib/moby.o: libc/testlib/moby.txt

# these assembly files are safe to build on aarch64
o/$(MODE)/libc/testlib/bench.o: libc/testlib/bench.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/blocktronics.o: libc/testlib/blocktronics.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/fixture.o: libc/testlib/fixture.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/hyperion.o: libc/testlib/hyperion.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/moby.o: libc/testlib/moby.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/polluteregisters.o: libc/testlib/polluteregisters.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/testcase.o: libc/testlib/testcase.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<
o/$(MODE)/libc/testlib/viewables.o: libc/testlib/viewables.S
	@$(COMPILE) -AOBJECTIFY.S $(OBJECTIFY.S) $(OUTPUT_OPTION) -c $<

#───────────────────────────────────────────────────────────────────────────────

LIBC_TESTLIB_ARTIFACTS += LIBC_TESTLIB_RUNNER_A
LIBC_TESTLIB_RUNNER = $(LIBC_TESTLIB_RUNNER_A_DEPS) $(LIBC_TESTLIB_RUNNER_A)
LIBC_TESTLIB_RUNNER_A = o/$(MODE)/libc/testlib/runner.a
LIBC_TESTLIB_RUNNER_A_SRCS = libc/testlib/runner.c
LIBC_TESTLIB_RUNNER_A_CHECKS = $(LIBC_TESTLIB_RUNNER_A).pkg

LIBC_TESTLIB_RUNNER_A_OBJS =					\
	$(LIBC_TESTLIB_RUNNER_A_SRCS:%.c=o/$(MODE)/%.o)

LIBC_TESTLIB_RUNNER_A_DIRECTDEPS =				\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
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
	o/$(MODE)/libc/testlib/testmain.o

LIBC_TESTMAIN_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_SYSV						\
	LIBC_SYSV_CALLS						\
	LIBC_TESTLIB						\
	LIBC_TESTLIB_RUNNER					\
	THIRD_PARTY_DLMALLOC					\
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
$(LIBC_TESTLIB_OBJS): $(BUILD_FILES) libc/testlib/BUILD.mk

.PHONY: o/$(MODE)/libc/testlib
o/$(MODE)/libc/testlib: $(LIBC_TESTLIB_LIBS) $(LIBC_TESTLIB_CHECKS)
