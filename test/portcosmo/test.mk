#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_PORTCOSMO

TEST_PORTCOSMO_SRCS := $(wildcard test/portcosmo/*.c)
TEST_PORTCOSMO_SRCS_TEST = $(filter %_test.c,$(TEST_PORTCOSMO_SRCS))

TEST_PORTCOSMO_OBJS =					\
	$(TEST_PORTCOSMO_SRCS:%.c=o/$(MODE)/%.o)

TEST_PORTCOSMO_COMS =					\
	$(TEST_PORTCOSMO_SRCS_TEST:%.c=o/$(MODE)/%.com)

TEST_PORTCOSMO_BINS =					\
	$(TEST_PORTCOSMO_COMS)				\
	$(TEST_PORTCOSMO_COMS:%=%.dbg)

TEST_PORTCOSMO_TESTS =					\
	$(TEST_PORTCOSMO_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_PORTCOSMO_CHECKS =					\
	$(TEST_PORTCOSMO_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_PORTCOSMO_DIRECTDEPS =				\
	LIBC_FMT					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_PORTCOSMO_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_PORTCOSMO_DIRECTDEPS),$($(x))))

o/$(MODE)/test/portcosmo/%.o: \
	CFLAGS += -fportcosmo

o/$(MODE)/test/portcosmo/portcosmo.pkg:			\
		$(TEST_PORTCOSMO_OBJS)			\
		$(foreach x,$(TEST_PORTCOSMO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/portcosmo/%.com.dbg:			\
		$(TEST_PORTCOSMO_DEPS)			\
		o/$(MODE)/test/portcosmo/%.o		\
		o/$(MODE)/test/portcosmo/constants.o		\
		o/$(MODE)/test/portcosmo/portcosmo.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/portcosmo
o/$(MODE)/test/portcosmo:				\
		$(TEST_PORTCOSMO_BINS)			\
		$(TEST_PORTCOSMO_CHECKS)
