#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_NET_HTTP

TEST_NET_HTTP_SRCS := $(wildcard test/net/http/*.c)
TEST_NET_HTTP_SRCS_TEST = $(filter %_test.c,$(TEST_NET_HTTP_SRCS))
TEST_NET_HTTP_BINS = $(TEST_NET_HTTP_COMS) $(TEST_NET_HTTP_COMS:%=%.dbg)

TEST_NET_HTTP_OBJS =						\
	$(TEST_NET_HTTP_SRCS:%.c=o/$(MODE)/%.o)

TEST_NET_HTTP_COMS =						\
	$(TEST_NET_HTTP_SRCS:%.c=o/$(MODE)/%.com)

TEST_NET_HTTP_TESTS =						\
	$(TEST_NET_HTTP_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_NET_HTTP_CHECKS =						\
	$(TEST_NET_HTTP_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_NET_HTTP_DIRECTDEPS =					\
	NET_HTTP						\
	LIBC_LOG						\
	LIBC_TESTLIB						\
	THIRD_PARTY_MBEDTLS

TEST_NET_HTTP_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_NET_HTTP_DIRECTDEPS),$($(x))))

o/$(MODE)/test/net/http/http.pkg:				\
		$(TEST_NET_HTTP_OBJS)				\
		$(foreach x,$(TEST_NET_HTTP_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/net/http/%.com.dbg:				\
		$(TEST_NET_HTTP_DEPS)				\
		o/$(MODE)/test/net/http/%.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/net/http
o/$(MODE)/test/net/http:					\
		$(TEST_NET_HTTP_BINS)				\
		$(TEST_NET_HTTP_CHECKS)
