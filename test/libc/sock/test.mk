#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_SOCK

TEST_LIBC_SOCK_SRCS := $(wildcard test/libc/sock/*.c)
TEST_LIBC_SOCK_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_SOCK_SRCS))

TEST_LIBC_SOCK_OBJS =						\
	$(TEST_LIBC_SOCK_SRCS:%=o/$(MODE)/%.zip.o)		\
	$(TEST_LIBC_SOCK_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_SOCK_COMS =						\
	$(TEST_LIBC_SOCK_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_SOCK_BINS =						\
	$(TEST_LIBC_SOCK_COMS)					\
	$(TEST_LIBC_SOCK_COMS:%=%.dbg)

TEST_LIBC_SOCK_TESTS =						\
	$(TEST_LIBC_SOCK_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_SOCK_CHECKS =						\
	$(TEST_LIBC_SOCK_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_SOCK_DIRECTDEPS =					\
	LIBC_CALLS						\
	LIBC_CALLS_HEFTY					\
	LIBC_STDIO						\
	LIBC_FMT						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_SOCK						\
	LIBC_STUBS						\
	LIBC_SYSV						\
	LIBC_TESTLIB						\
	LIBC_X							\
	TOOL_DECODE_LIB

TEST_LIBC_SOCK_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_LIBC_SOCK_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/sock/sock.pkg:				\
		$(TEST_LIBC_SOCK_OBJS)				\
		$(foreach x,$(TEST_LIBC_SOCK_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/sock/%.com.dbg:				\
		$(TEST_LIBC_SOCK_DEPS)				\
		o/$(MODE)/test/libc/sock/%.o			\
		o/$(MODE)/test/libc/sock/sock.pkg		\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE)
	@$(APELINK)

$(TEST_LIBC_SOCK_OBJS): test/libc/sock/test.mk

.PHONY: o/$(MODE)/test/libc/sock
o/$(MODE)/test/libc/sock:					\
		$(TEST_LIBC_SOCK_BINS)				\
		$(TEST_LIBC_SOCK_CHECKS)
