#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_ZIPOS

TEST_LIBC_ZIPOS_SRCS := $(wildcard test/libc/zipos/*.c)
TEST_LIBC_ZIPOS_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_ZIPOS_SRCS))

TEST_LIBC_ZIPOS_OBJS =						\
	$(TEST_LIBC_ZIPOS_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_ZIPOS_COMS =						\
	$(TEST_LIBC_ZIPOS_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_ZIPOS_BINS =						\
	$(TEST_LIBC_ZIPOS_COMS)					\
	$(TEST_LIBC_ZIPOS_COMS:%=%.dbg)

TEST_LIBC_ZIPOS_TESTS =						\
	$(TEST_LIBC_ZIPOS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_ZIPOS_CHECKS =					\
	$(TEST_LIBC_ZIPOS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_ZIPOS_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STR						\
	LIBC_STUBS						\
	LIBC_THREAD						\
	LIBC_SYSV						\
	LIBC_ZIPOS						\
	LIBC_TIME						\
	LIBC_TESTLIB						\
	THIRD_PARTY_ZLIB

TEST_LIBC_ZIPOS_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_ZIPOS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/zipos/zipos.pkg:				\
		$(TEST_LIBC_ZIPOS_OBJS)				\
		$(foreach x,$(TEST_LIBC_ZIPOS_DIRECTDEPS),$($(x)_A).pkg)

#o/$(MODE)/libc/testlib/hyperion.txt.zip.o: private ZIPOBJ_FLAGS += -0

o/$(MODE)/test/libc/zipos/%.com.dbg:				\
		$(TEST_LIBC_ZIPOS_DEPS)				\
		o/$(MODE)/test/libc/zipos/%.o			\
		o/$(MODE)/test/libc/zipos/zipos.pkg		\
		o/$(MODE)/libc/testlib/hyperion.txt.zip.o	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/zipos
o/$(MODE)/test/libc/zipos:					\
		$(TEST_LIBC_ZIPOS_BINS)				\
		$(TEST_LIBC_ZIPOS_CHECKS)
