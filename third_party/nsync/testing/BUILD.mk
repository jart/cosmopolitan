#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += THIRD_PARTY_NSYNC_TESTING

THIRD_PARTY_NSYNC_TESTING_LIB = $(THIRD_PARTY_NSYNC_TESTING_DEPS) $(THIRD_PARTY_NSYNC_TESTING_A)
THIRD_PARTY_NSYNC_TESTING_A = o/$(MODE)/third_party/nsync/testing/lib.a
THIRD_PARTY_NSYNC_TESTING_FILES = $(wildcard third_party/nsync/testing/*)
THIRD_PARTY_NSYNC_TESTING_SRCS = $(filter %.c,$(THIRD_PARTY_NSYNC_TESTING_FILES))
THIRD_PARTY_NSYNC_TESTING_HDRS = $(filter %.h,$(THIRD_PARTY_NSYNC_TESTING_FILES))
THIRD_PARTY_NSYNC_TESTING_SRCS_TEST = $(filter %_test.c,$(THIRD_PARTY_NSYNC_TESTING_SRCS))
THIRD_PARTY_NSYNC_TESTING_OBJS = $(THIRD_PARTY_NSYNC_TESTING_SRCS:%.c=o/$(MODE)/%.o)
THIRD_PARTY_NSYNC_TESTING_COMS = $(THIRD_PARTY_NSYNC_TESTING_SRCS_TEST:%.c=o/$(MODE)/%.com)
THIRD_PARTY_NSYNC_TESTING_BINS = $(THIRD_PARTY_NSYNC_TESTING_COMS) $(THIRD_PARTY_NSYNC_TESTING_COMS:%=%.dbg)
THIRD_PARTY_NSYNC_TESTING_TESTS_ = $(THIRD_PARTY_NSYNC_TESTING_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)
THIRD_PARTY_NSYNC_TESTING_CHECKS_ = $(THIRD_PARTY_NSYNC_TESTING_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

THIRD_PARTY_NSYNC_TESTING_DIRECTDEPS =				\
	LIBC_CALLS						\
	LIBC_FMT						\
	LIBC_INTRIN						\
	LIBC_LOG						\
	LIBC_MEM						\
	LIBC_NEXGEN32E						\
	LIBC_RUNTIME						\
	LIBC_STDIO						\
	LIBC_STR						\
	LIBC_SYSV						\
	LIBC_THREAD						\
	THIRD_PARTY_NSYNC					\
	THIRD_PARTY_NSYNC_MEM

THIRD_PARTY_NSYNC_TESTING_DEPS :=				\
	$(call uniq,$(foreach x,$(THIRD_PARTY_NSYNC_TESTING_DIRECTDEPS),$($(x))))

$(THIRD_PARTY_NSYNC_TESTING_A):					\
		third_party/nsync/testing/			\
		$(THIRD_PARTY_NSYNC_TESTING_A).pkg		\
		$(THIRD_PARTY_NSYNC_TESTING_OBJS)

$(THIRD_PARTY_NSYNC_TESTING_A).pkg:				\
		$(THIRD_PARTY_NSYNC_TESTING_OBJS)		\
		$(foreach x,$(THIRD_PARTY_NSYNC_TESTING_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/third_party/nsync/testing/%_test.com.dbg:		\
		$(THIRD_PARTY_NSYNC_TESTING_DEPS)		\
		$(THIRD_PARTY_NSYNC_TESTING_A)			\
		o/$(MODE)/third_party/nsync/testing/%_test.o	\
		$(THIRD_PARTY_NSYNC_TESTING_A).pkg		\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

$(THIRD_PARTY_NSYNC_TESTING_OBJS): third_party/nsync/testing/BUILD.mk
o/$(MODE)/third_party/nsync/testing/mu_test.com.runs: private QUOTA = -C64

.PHONY: o/$(MODE)/third_party/nsync/testing
o/$(MODE)/third_party/nsync/testing:				\
	$(THIRD_PARTY_NSYNC_TESTING_CHECKS_)			\
	$(THIRD_PARTY_NSYNC_TESTING_BINS_)

.PHONY: o/$(MODE)/third_party/nsync/test
o/$(MODE)/third_party/nsync/test:				\
	$(THIRD_PARTY_NSYNC_TESTING_CHECKS_)			\
	$(THIRD_PARTY_NSYNC_TESTING_TESTS_)
