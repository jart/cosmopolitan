#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBCXX

TEST_LIBCXX_FILES := $(wildcard test/libcxx/*)
TEST_LIBCXX_SRCS = $(filter %.cc,$(TEST_LIBCXX_FILES))
TEST_LIBCXX_OBJS = $(TEST_LIBCXX_SRCS:%.cc=o/$(MODE)/%.o)
TEST_LIBCXX_COMS = $(TEST_LIBCXX_OBJS:%.o=%.com)
TEST_LIBCXX_BINS = $(TEST_LIBCXX_COMS) $(TEST_LIBCXX_COMS:%=%.dbg)
TEST_LIBCXX_CHECKS = $(TEST_LIBCXX_COMS:%=%.runs)
TEST_LIBCXX_TESTS = $(TEST_LIBCXX_COMS:%=%.ok)

TEST_LIBCXX_DIRECTDEPS =				\
	LIBC_CALLS					\
	LIBC_INTRIN					\
	LIBC_LOG					\
	LIBC_NEXGEN32E					\
	LIBC_RUNTIME					\
	LIBC_STDIO					\
	LIBC_SYSV					\
	LIBC_THREAD					\
	LIBC_TINYMATH					\
	THIRD_PARTY_LIBCXX				\
	THIRD_PARTY_OPENMP

TEST_LIBCXX_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBCXX_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libcxx/libcxx.pkg:			\
		$(TEST_LIBCXX_OBJS)			\
		$(foreach x,$(TEST_LIBCXX_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libcxx/%.com.dbg:			\
		$(TEST_LIBCXX_DEPS)			\
		o/$(MODE)/test/libcxx/%.o		\
		o/$(MODE)/test/libcxx/libcxx.pkg	\
		$(CRT)					\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_LIBCXX_OBJS): private CCFLAGS += -fexceptions -frtti

o/$(MODE)/test/libcxx/openmp_test.o: private CXXFLAGS += -fopenmp
o/$(MODE)/test/libcxx/openmp_test.com.runs: private QUOTA += -C100

.PHONY: o/$(MODE)/test/libcxx
o/$(MODE)/test/libcxx:					\
		$(TEST_LIBCXX_BINS)			\
		$(TEST_LIBCXX_CHECKS)
