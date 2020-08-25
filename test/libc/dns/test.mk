#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_DNS

TEST_LIBC_DNS_SRCS := $(wildcard test/libc/dns/*.c)
TEST_LIBC_DNS_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_DNS_SRCS))

TEST_LIBC_DNS_OBJS =					\
	$(TEST_LIBC_DNS_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_DNS_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_DNS_COMS =					\
	$(TEST_LIBC_DNS_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_DNS_BINS =					\
	$(TEST_LIBC_DNS_COMS)				\
	$(TEST_LIBC_DNS_COMS:%=%.dbg)

TEST_LIBC_DNS_TESTS =					\
	$(TEST_LIBC_DNS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_DNS_CHECKS =					\
	$(TEST_LIBC_DNS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_DNS_DIRECTDEPS =				\
	LIBC_CALLS_HEFTY				\
	LIBC_DNS					\
	LIBC_FMT					\
	LIBC_LOG					\
	LIBC_MEM					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_SOCK					\
	LIBC_STDIO					\
	LIBC_STR					\
	LIBC_STUBS					\
	LIBC_SYSV					\
	LIBC_TESTLIB					\
	LIBC_X

TEST_LIBC_DNS_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_DNS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/dns/dns.pkg:			\
		$(TEST_LIBC_DNS_OBJS)			\
		$(foreach x,$(TEST_LIBC_DNS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/dns/%.com.dbg:			\
		$(TEST_LIBC_DNS_DEPS)			\
		o/$(MODE)/test/libc/dns/%.o		\
		o/$(MODE)/test/libc/dns/dns.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/dns
o/$(MODE)/test/libc/dns:				\
		$(TEST_LIBC_DNS_BINS)			\
		$(TEST_LIBC_DNS_CHECKS)
