#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_CTL

TEST_CTL_FILES := $(wildcard test/ctl/*)
TEST_CTL_SRCS = $(filter %.cc,$(TEST_CTL_FILES))
TEST_CTL_OBJS = $(TEST_CTL_SRCS:%.cc=o/$(MODE)/%.o)
TEST_CTL_COMS = $(TEST_CTL_OBJS:%.o=%)
TEST_CTL_BINS = $(TEST_CTL_COMS) $(TEST_CTL_COMS:%=%.dbg)
TEST_CTL_CHECKS = $(TEST_CTL_COMS:%=%.runs)
TEST_CTL_TESTS = $(TEST_CTL_COMS:%=%.ok)

TEST_CTL_DIRECTDEPS =				\
	CTL					\
	LIBC_CALLS				\
	LIBC_INTRIN				\
	LIBC_LOG				\
	LIBC_MEM				\
	LIBC_NEXGEN32E				\
	LIBC_PROC				\
	LIBC_STDIO				\
	LIBC_SYSTEM				\
	LIBC_THREAD				\
	THIRD_PARTY_LIBCXX			\
	THIRD_PARTY_LIBCXXABI			\
	THIRD_PARTY_LIBUNWIND			\

TEST_CTL_DEPS :=				\
	$(call uniq,$(foreach x,$(TEST_CTL_DIRECTDEPS),$($(x))))

o/$(MODE)/test/ctl/ctl.pkg:			\
		$(TEST_CTL_OBJS)		\
		$(foreach x,$(TEST_CTL_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/ctl/%.dbg:			\
		$(TEST_CTL_DEPS)		\
		o/$(MODE)/test/ctl/%.o		\
		o/$(MODE)/test/ctl/ctl.pkg	\
		$(CRT)				\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(TEST_CTL_OBJS): private			\
		OVERRIDE_CXXFLAGS +=		\
			-fexceptions		\

.PHONY: o/$(MODE)/test/ctl
o/$(MODE)/test/ctl:				\
		$(TEST_CTL_BINS)		\
		$(TEST_CTL_CHECKS)
