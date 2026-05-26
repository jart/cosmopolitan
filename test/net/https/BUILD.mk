#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_NET_HTTPS

TEST_NET_HTTPS_SRCS := $(wildcard test/net/https/*.c)
TEST_NET_HTTPS_SRCS_TEST = $(filter %_test.c,$(TEST_NET_HTTPS_SRCS))
TEST_NET_HTTPS_BINS = $(TEST_NET_HTTPS_COMS) $(TEST_NET_HTTPS_COMS:%=%.dbg)

TEST_NET_HTTPS_OBJS =						\
	$(TEST_NET_HTTPS_SRCS:%.c=o/$(MODE)/%.o)

TEST_NET_HTTPS_COMS =						\
	$(TEST_NET_HTTPS_SRCS:%.c=o/$(MODE)/%)

TEST_NET_HTTPS_TESTS =						\
	$(TEST_NET_HTTPS_SRCS_TEST:%.c=o/$(MODE)/%.ok)

TEST_NET_HTTPS_CHECKS =						\
	$(TEST_NET_HTTPS_SRCS_TEST:%.c=o/$(MODE)/%.runs)

TEST_NET_HTTPS_DIRECTDEPS =					\
	LIBC_LOG						\
	LIBC_TESTLIB						\
	NET_HTTPS						\
	THIRD_PARTY_ARGON2					\
	THIRD_PARTY_HACLSTAR					\
	THIRD_PARTY_MBEDTLS4					\

TEST_NET_HTTPS_DEPS :=						\
	$(call uniq,$(foreach x,$(TEST_NET_HTTPS_DIRECTDEPS),$($(x))))

o/$(MODE)/test/net/https/https.pkg:				\
		$(TEST_NET_HTTPS_OBJS)				\
		$(foreach x,$(TEST_NET_HTTPS_DIRECTDEPS),$($(x)_A).pkg)

# All test/net/https sources include net/https/https.h which pulls in
# mbedTLS4 headers that use relative includes — provide the search paths.
$(TEST_NET_HTTPS_OBJS): private					\
		CPPFLAGS +=					\
			-iquotethird_party/mbedtls4/generated	\
			-Ithird_party/mbedtls4/include		\
			-Ithird_party/mbedtls4/tf-psa-crypto/include

o/$(MODE)/test/net/https/%.dbg:					\
		$(TEST_NET_HTTPS_DEPS)				\
		o/$(MODE)/test/net/https/%.o			\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# mbedtls4_test.c uses the PSA-first mbedTLS4 headers directly. The three -I
# flags let mbedTLS4's internal headers resolve sibling includes.
o/$(MODE)/test/net/https/mbedtls4_test.o: private		\
		CPPFLAGS +=					\
			-DMBEDTLS_ALLOW_PRIVATE_ACCESS		\
			-iquotethird_party/mbedtls4/generated	\
			-Ithird_party/mbedtls4/include		\
			-Ithird_party/mbedtls4/tf-psa-crypto/include \
			-Ithird_party/mbedtls4/tf-psa-crypto/drivers/builtin/include

o/$(MODE)/test/net/https/getciphersuite_test.o: private		\
		CPPFLAGS +=					\
			-iquotethird_party/mbedtls4/generated	\
			-Ithird_party/mbedtls4/include		\
			-Ithird_party/mbedtls4/tf-psa-crypto/include

.PHONY: o/$(MODE)/test/net/https
o/$(MODE)/test/net/https:					\
		$(TEST_NET_HTTPS_BINS)				\
		$(TEST_NET_HTTPS_CHECKS)
