#-*-mode:makefile-gmake;indent-tabs-mode:t;tab-width:8;coding:utf-8-*-┐
#── vi: set noet ft=make ts=8 sw=8 fenc=utf-8 :vi ────────────────────┘

PKGS += TEST_LIBC_NEXGEN32E

TEST_LIBC_NEXGEN32E_SRCS :=					\
	$(wildcard test/libc/nexgen32e/*.c)
TEST_LIBC_NEXGEN32E_SRCS_TEST =					\
	$(filter %_test.c,$(TEST_LIBC_NEXGEN32E_SRCS))

TEST_LIBC_NEXGEN32E_OBJS =					\
	$(TEST_LIBC_NEXGEN32E_SRCS:%.c=o/$(MODE)/%.o)

TEST_LIBC_NEXGEN32E_COMS =					\
	$(TEST_LIBC_NEXGEN32E_SRCS:%.c=o/$(MODE)/%.com)

TEST_LIBC_NEXGEN32E_BINS =					\
	$(TEST_LIBC_NEXGEN32E_COMS)				\
	$(TEST_LIBC_NEXGEN32E_COMS:%=%.dbg)

TEST_LIBC_NEXGEN32E_TESTS =					\
	$(TEST_LIBC_NEXGEN32E_SRCS_TEST:%.c=o/$(MODE)/%.com.ok)

TEST_LIBC_NEXGEN32E_CHECKS =					\
	$(TEST_LIBC_NEXGEN32E_SRCS_TEST:%.c=o/$(MODE)/%.com.runs)

TEST_LIBC_NEXGEN32E_DIRECTDEPS =				\
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
	LIBC_TESTLIB						\
	LIBC_X							\
	TOOL_VIZ_LIB						\
	THIRD_PARTY_XED

TEST_LIBC_NEXGEN32E_DEPS :=					\
	$(call uniq,$(foreach x,$(TEST_LIBC_NEXGEN32E_DIRECTDEPS),$($(x))))

o/$(MODE)/test/libc/nexgen32e/nexgen32e.pkg:			\
		$(TEST_LIBC_NEXGEN32E_OBJS)			\
		$(foreach x,$(TEST_LIBC_NEXGEN32E_DIRECTDEPS),$($(x)_A).pkg)

o/$(MODE)/test/libc/nexgen32e/%.com.dbg:			\
		$(TEST_LIBC_NEXGEN32E_DEPS)			\
		o/$(MODE)/test/libc/nexgen32e/%.o		\
		o/$(MODE)/test/libc/nexgen32e/nexgen32e.pkg	\
		$(LIBC_TESTMAIN)				\
		$(CRT)						\
		$(APE_NO_MODIFY_SELF)
	@$(APELINK)

# we can't run this test on openbsd because rwx memory isn't allowed
o/$(MODE)/test/libc/nexgen32e/stackrwx_test.com.ok:		\
		o/$(MODE)/tool/build/runit.com			\
		o/$(MODE)/tool/build/runitd.com			\
		o/$(MODE)/test/libc/nexgen32e/stackrwx_test.com
	@$(COMPILE) -wATEST -tT$@ $^ $(filter-out openbsd,$(HOSTS))

$(TEST_LIBC_NEXGEN32E_OBJS): private				\
	DEFAULT_CCFLAGS +=					\
		-fno-builtin

.PHONY: o/$(MODE)/test/libc/nexgen32e
o/$(MODE)/test/libc/nexgen32e:					\
		$(TEST_LIBC_NEXGEN32E_BINS)			\
		$(TEST_LIBC_NEXGEN32E_CHECKS)
