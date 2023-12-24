#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_NET_HTTPS

TEST_NET_HTTPS_SRCS := $(wildcard test/net/https/*.c)
TEST_NET_HTTPS_SRCS_TEST = $(filter %_test.c,$(TEST_NET_HTTPS_SRCS))
TEST_NET_HTTPS_BINS = $(TEST_NET_HTTPS_COMS) $(TEST_NET_HTTPS_COMS:%=%.dbg)

TEST_NET_HTTPS_OBJS =						\
	$(TEST_NET_HTTPS_SRCS:%.c=o/$(MODE)/%.o)

TEST_NET_HTTPS_COMS =						\
	$(TEST_NET_HTTPS_SRCS:%.c=o/$(MODE)/%.com)

TEST_NET_HTTPS_TESTS =						\
	$(TEST_NET_HTTPS_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_NET_HTTPS_CHECKS =						\
	$(TEST_NET_HTTPS_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_NET_HTTPS_DIRECTDEPS =					\
	NET_HTTPS						\
	LIBC_LOG						\
	LIBC_TESTLIB						\
	THIRD_PARTY_ARGON2					\
	THIRD_PARTY_MBEDTLS

TEST_NET_HTTPS_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_NET_HTTPS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/net/https/https.pkg:				\
		$(TEST_NET_HTTPS_OBJS)				\
		$(foreach x,$(TEST_NET_HTTPS_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/net/https/%.com.dbg:				\
		$(TEST_NET_HTTPS_DEPS)				\
		o/$(MODE)/test/net/https/%.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/net/https
o/$(MODE)/test/net/https:					\
		$(TEST_NET_HTTPS_BINS)				\
		$(TEST_NET_HTTPS_CHECKS)
