#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#───vi: set et ft=make ts=8 tw=8 fenc=utf-8 :vi───────────────────────┘

PKGS += TEST_LIBC_CRYPTO

TEST_LIBC_CRYPTO_SRCS := $(wildcard test/libc/crypto/*.c)
TEST_LIBC_CRYPTO_SRCS_TEST = $(filter %_test.c,$(TEST_LIBC_CRYPTO_SRCS))
TEST_LIBC_CRYPTO_COMS = $(TEST_LIBC_CRYPTO_OBJS:%.o=%.com)

TEST_LIBC_CRYPTO_OBJS =					\
	$(TEST_LIBC_CRYPTO_SRCS:%=o/$(MODE)/%.zip.o)	\
	$(TEST_LIBC_CRYPTO_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_CRYPTO_BINS =					\
	$(TEST_LIBC_CRYPTO_COMS)			\
	$(TEST_LIBC_CRYPTO_COMS:%=%.dbg)

TEST_LIBC_CRYPTO_TESTS =				\
	$(TEST_LIBC_CRYPTO_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_CRYPTO_CHECKS =				\
	$(TEST_LIBC_CRYPTO_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_CRYPTO_DIRECTDEPS =				\
	LIBC_CRYPTO					\
	LIBC_RUNTIME					\
	LIBC_FMT					\
	LIBC_NEXGEN32E					\
	LIBC_STUBS					\
	LIBC_TESTLIB

TEST_LIBC_CRYPTO_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_LIBC_CRYPTO_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/crypto/crypto.pkg:			\
		$(TEST_LIBC_CRYPTO_OBJS)		\
		$(foreach x,$(TEST_LIBC_CRYPTO_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/crypto/%.com.dbg:			\
		$(TEST_LIBC_CRYPTO_DEPS)		\
		o/$(MODE)/test/libc/crypto/%.o		\
		o/$(MODE)/test/libc/crypto/crypto.pkg	\
		$(LIBC_TESTMAIN)			\
		$(CRT)					\
		$(APE)
	@$(APELINK)

.PHONY: o/$(MODE)/test/libc/crypto
o/$(MODE)/test/libc/crypto:				\
		$(TEST_LIBC_CRYPTO_BINS)		\
		$(TEST_LIBC_CRYPTO_CHECKS)
