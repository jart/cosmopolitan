#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_NET_FINGER

TEST_NET_FINGER_SRCS := $(wildcard test/net/finger/*.c)
TEST_NET_FINGER_SRCS_TEST = $(filter %_test.c,$(TEST_NET_FINGER_SRCS))
TEST_NET_FINGER_BINS = $(TEST_NET_FINGER_COMS) $(TEST_NET_FINGER_COMS:%=%.dbg)

TEST_NET_FINGER_OBJS =						\
	$(TEST_NET_FINGER_SRCS:%.c=o/$(MODE)/%.o)

TEST_NET_FINGER_COMS =						\
	$(TEST_NET_FINGER_SRCS:%.c=o/$(MODE)/%.com)

TEST_NET_FINGER_TESTS =						\
	$(TEST_NET_FINGER_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_NET_FINGER_CHECKS =					\
	$(TEST_NET_FINGER_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_NET_FINGER_DIRECTDEPS =					\
	NET_FINGER						\
	LIBC_LOG						\
	LIBC_TESTLIB						\
	THIRD_PARTY_MBEDTLS

TEST_NET_FINGER_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_NET_FINGER_DIRECTDEPS),$($(x))))

o/$(MODE)/test/net/finger/finger.pkg:				\
		$(TEST_NET_FINGER_OBJS)				\
		$(foreach x,$(TEST_NET_FINGER_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/net/finger/%.com.dbg:				\
		$(TEST_NET_FINGER_DEPS)				\
		o/$(MODE)/test/net/finger/%.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

.PHONY: o/$(MODE)/test/net/finger
o/$(MODE)/test/net/finger:					\
		$(TEST_NET_FINGER_BINS)				\
		$(TEST_NET_FINGER_CHECKS)
